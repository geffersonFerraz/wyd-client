package main

import (
	"bytes"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"os"
	"path/filepath"
	"strings"
	"sync"
	"time"
)

// Arquivos que o launcher mantém iguais aos da porta pública do painel. O
// endereço vem do próprio serverlist.bin local: a entrada [grupo][0] é a URL do
// servNN.htm, servida pelo mesmo listener.
//
// A cada abertura ele lê o manifest.json do painel (nome, tamanho e SHA-256 de
// cada arquivo) e só baixa o que mudou. Um painel antigo, sem manifesto, faz o
// launcher baixar tudo, como antes.
const (
	serverListFile  = "serverlist.bin"
	itemListFile    = "ItemList.bin"
	serverListGroup = 11 * 64              // MAX_SERVERNUMBER x 64
	serverListSize  = 10 * serverListGroup // MAX_SERVERGROUP grupos
	itemListSize    = 6500 * 156           // MAX_ITEMLIST x sizeof(STRUCT_ITEMLIST)
	launcherPage    = "/launcher/"
	manifestPath    = "/manifest.json"
	manifestVersion = 1
	manifestMaxSize = 64 * 1024
	manifestTimeout = 10 * time.Second
	// stateFile guarda o que o launcher baixou por último. O serverlist.bin em
	// disco pode ter o IP das Configurações, e aí o hash dele nunca é o do
	// manifesto: por isso o estado tem o hash do servidor e o do arquivo gravado.
	stateFile       = "launcher-manifest.json"
	downloadTimeout = 60 * time.Second
	pageTimeout     = 5 * time.Second
	readChunk       = 32 * 1024
)

// serverListKey é o szList de BASE_InitializeServerList.
var serverListKey = [64]byte{
	0xa4, 0xa1, 0xa4, 0xa4, 0xa4, 0xa7, 0xa4, 0xa9, 0xa4, 0xb1, 0xa4, 0xb2, 0xa4, 0xb5, 0xa4, 0xb7,
	0xa4, 0xb8, 0xa4, 0xba, 0xa4, 0xbb, 0xa4, 0xbc, 0xa4, 0xbd, 0xa4, 0xbe, 0xa4, 0xbf, 0xa4, 0xc1,
	0xa4, 0xc3, 0xa4, 0xc5, 0xa4, 0xc7, 0xa4, 0xcb, 0xa4, 0xcc, 0xa4, 0xd0, 0xa4, 0xd1, 0xa4, 0xd3,
	0xa4, 0xbf, 0xa4, 0xc4, 0xa4, 0xd3, 0xa4, 0xc7, 0xa4, 0xcc, 0xb0, 0xa1, 0xb3, 0xaa, 0xb4, 0xd9,
}

// serverBase devolve "http://host[:porta]" do primeiro grupo com URL de status.
func serverBase(data []byte) (string, error) {
	if len(data) != serverListSize {
		return "", fmt.Errorf("%s com tamanho inesperado: %d bytes", serverListFile, len(data))
	}
	for group := 0; group < serverListSize/serverListGroup; group++ {
		entry := data[group*serverListGroup : group*serverListGroup+len(serverListKey)]
		plain := make([]byte, len(entry))
		for i, b := range entry {
			plain[i] = b - serverListKey[len(serverListKey)-1-i]
		}
		text, _, _ := strings.Cut(string(plain), "\x00")
		u, err := url.Parse(text)
		if err == nil && (u.Scheme == "http" || u.Scheme == "https") && u.Host != "" {
			return u.Scheme + "://" + u.Host, nil
		}
	}
	return "", fmt.Errorf("%s sem endereço do servidor", serverListFile)
}

type updateFile struct {
	name  string
	path  string
	size  int
	check func([]byte) error
	// pin ajusta o arquivo baixado ao endereço das Configurações (opcional).
	pin func(data []byte, address string) []byte
}

var updateFiles = []updateFile{
	{serverListFile, "/serverlist.bin", serverListSize, func(b []byte) error {
		_, err := serverBase(b) // Sem endereço, a próxima abertura não acharia o servidor.
		return err
	}, rewriteServerList},
	{itemListFile, "/itemlist.bin", itemListSize, func([]byte) error { return nil }, nil},
}

// Progress é o estado que a janela mostra.
type Progress struct {
	File, Total float64 // 0 a 1
	Status      string
	Done, Ready bool // Ready: os arquivos locais servem para jogar.
}

type updater struct {
	dir    string
	client *http.Client
	report func(Progress)
}

func newUpdater(dir string, report func(Progress)) *updater {
	return &updater{dir: dir, client: &http.Client{Timeout: downloadTimeout}, report: report}
}

// localPath acha o arquivo sem olhar maiúsculas: a release traz "Itemlist.bin" e
// o cliente abre "ItemList.bin". Num disco do Linux, gravar com a outra grafia
// deixaria dois arquivos, e o Wine poderia ler o antigo.
func localPath(dir, name string) string {
	entries, err := os.ReadDir(dir)
	if err == nil {
		for _, e := range entries {
			if strings.EqualFold(e.Name(), name) {
				return filepath.Join(dir, e.Name())
			}
		}
	}
	return filepath.Join(dir, name)
}

// replaceFile troca o arquivo de uma vez, para uma falha não deixá-lo pela metade.
func replaceFile(path string, data []byte) error {
	tmp := path + ".tmp"
	if err := os.WriteFile(tmp, data, 0o644); err != nil {
		return fmt.Errorf("não foi possível gravar %s: %w", path, err)
	}
	if err := os.Rename(tmp, path); err != nil {
		os.Remove(tmp)
		return fmt.Errorf("não foi possível gravar %s: %w", path, err)
	}
	return nil
}

func (u *updater) download(address string, size int, progress func(float64)) ([]byte, error) {
	res, err := u.client.Get(address)
	if err != nil {
		return nil, err
	}
	defer res.Body.Close()
	if res.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("%s: %s", address, res.Status)
	}
	body := io.LimitReader(res.Body, int64(size)+1)
	data := make([]byte, 0, size+1)
	chunk := make([]byte, readChunk)
	for {
		n, err := body.Read(chunk)
		data = append(data, chunk[:n]...)
		progress(min(float64(len(data))/float64(size), 1))
		if errors.Is(err, io.EOF) {
			break
		}
		if err != nil {
			return nil, err
		}
	}
	if len(data) != size {
		return nil, fmt.Errorf("%s: esperava %d bytes", address, size)
	}
	return data, nil
}

// manifestFile é uma entrada do manifest.json do painel.
type manifestFile struct {
	Name   string `json:"name"`
	Path   string `json:"path"`
	Size   int    `json:"size"`
	SHA256 string `json:"sha256"`
}

// fileState é o que ficou em disco no último download de um arquivo.
type fileState struct {
	Remote string `json:"remote"` // SHA-256 do arquivo no servidor
	Local  string `json:"local"`  // SHA-256 do arquivo gravado (difere do Remote com IP fixado)
	// Address é o endereço das Configurações aplicado ao arquivo. Apagar o
	// endereço tem de trazer de volta o arquivo do servidor.
	Address string `json:"address,omitempty"`
}

func hashOf(data []byte) string {
	sum := sha256.Sum256(data)
	return hex.EncodeToString(sum[:])
}

// fetchManifest devolve as entradas pelo caminho ("/itemlist.bin"). Qualquer
// falha devolve nil: sem manifesto, tudo é baixado.
func (u *updater) fetchManifest(base string) map[string]manifestFile {
	client := *u.client
	client.Timeout = manifestTimeout
	res, err := client.Get(base + manifestPath)
	if err != nil {
		return nil
	}
	defer res.Body.Close()
	if res.StatusCode != http.StatusOK {
		return nil
	}
	body, err := io.ReadAll(io.LimitReader(res.Body, manifestMaxSize+1))
	if err != nil || len(body) > manifestMaxSize {
		return nil
	}
	var doc struct {
		Version int            `json:"version"`
		Files   []manifestFile `json:"files"`
	}
	if json.Unmarshal(body, &doc) != nil || doc.Version != manifestVersion {
		return nil
	}
	files := make(map[string]manifestFile, len(doc.Files))
	for _, f := range doc.Files {
		if len(f.SHA256) == sha256.Size*2 {
			files[f.Path] = manifestFile{f.Name, f.Path, f.Size, strings.ToLower(f.SHA256)}
		}
	}
	return files
}

func (u *updater) loadState() map[string]fileState {
	state := map[string]fileState{}
	if data, err := os.ReadFile(filepath.Join(u.dir, stateFile)); err == nil {
		if json.Unmarshal(data, &state) != nil || state == nil {
			state = map[string]fileState{}
		}
	}
	return state
}

// saveState não devolve erro: sem o estado, a próxima abertura só baixa de novo.
func (u *updater) saveState(state map[string]fileState) {
	if data, err := json.MarshalIndent(state, "", "  "); err == nil {
		replaceFile(filepath.Join(u.dir, stateFile), data)
	}
}

// upToDate diz se o arquivo em disco já é o do manifesto: ou tem o mesmo hash,
// ou é o que o launcher gravou quando baixou essa mesma versão.
func (u *updater) upToDate(f updateFile, want manifestFile, last fileState, address string) bool {
	if want.Size != f.size {
		return false
	}
	data, err := os.ReadFile(localPath(u.dir, f.name))
	if err != nil || len(data) != f.size || f.check(data) != nil {
		return false
	}
	local := hashOf(data)
	if f.pin == nil || address == "" {
		address = ""
		if local == want.SHA256 {
			return true
		}
	}
	return last.Remote == want.SHA256 && last.Local == local && last.Address == address
}

// fetch baixa e grava o arquivo. Com manifesto, o conteúdo tem de bater com o
// hash anunciado: um download corrompido ou cortado não substitui a cópia local.
func (u *updater) fetch(base, address string, f updateFile, want *manifestFile, progress func(float64)) (fileState, error) {
	data, err := u.download(base+f.path, f.size, progress)
	if err != nil {
		return fileState{}, err
	}
	remote := hashOf(data)
	if want != nil && remote != want.SHA256 {
		return fileState{}, fmt.Errorf("%s: conteúdo diferente do manifesto", f.name)
	}
	if err := f.check(data); err != nil {
		return fileState{}, err
	}
	pinned := ""
	if f.pin != nil && address != "" {
		data, pinned = f.pin(data, address), address
	}
	if err := replaceFile(localPath(u.dir, f.name), data); err != nil {
		return fileState{}, err
	}
	return fileState{Remote: remote, Local: hashOf(data), Address: pinned}, nil
}

// localReady diz se os arquivos em disco bastam para abrir o jogo.
func (u *updater) localReady() bool {
	for _, f := range updateFiles {
		data, err := os.ReadFile(localPath(u.dir, f.name))
		if err != nil || len(data) != f.size || f.check(data) != nil {
			return false
		}
	}
	return true
}

// updateMu impede duas atualizações ao mesmo tempo: trocar o endereço nas
// Configurações dispara outra enquanto a primeira ainda pode estar rodando.
var updateMu sync.Mutex

// pinLocalList põe o endereço das Configurações no serverlist.bin local, para o
// jogo abrir no IP certo mesmo quando o download falhar.
func (u *updater) pinLocalList(address string) {
	path := localPath(u.dir, serverListFile)
	local, err := os.ReadFile(path)
	if err != nil || len(local) != serverListSize {
		return
	}
	if pinned := rewriteServerList(local, address); !bytes.Equal(pinned, local) {
		replaceFile(path, pinned)
	}
}

// run atualiza os arquivos na ordem, pulando os que o manifesto diz que já
// estão iguais. Se o servidor falhar, as cópias locais servem:
// uma queda do site não pode impedir quem já tem o jogo de abri-lo. O endereço
// vem das Configurações do launcher (Servidor.txt) ou, sem ele, do serverlist.bin.
func (u *updater) run() {
	updateMu.Lock()
	defer updateMu.Unlock()
	address := loadServerAddress(u.dir)
	var base string
	if address != "" {
		base = "http://" + address
		u.pinLocalList(address)
	} else {
		local, err := os.ReadFile(localPath(u.dir, serverListFile))
		if err != nil {
			u.report(Progress{Status: serverListFile + " não encontrado. Extraia o jogo novamente ou informe o servidor em Configuração.", Done: true})
			return
		}
		if base, err = serverBase(local); err != nil {
			u.report(Progress{Status: serverListFile + " inválido. Extraia o jogo novamente ou informe o servidor em Configuração.", Done: true})
			return
		}
	}
	u.report(Progress{Status: "Verificando atualizações..."})
	manifest := u.fetchManifest(base)
	state := u.loadState()
	changed := false
	defer func() {
		if changed {
			u.saveState(state)
		}
	}()
	for i, f := range updateFiles {
		var want *manifestFile
		if entry, ok := manifest[f.path]; ok {
			want = &entry
			if u.upToDate(f, entry, state[f.name], address) {
				u.report(Progress{File: 1, Total: float64(i+1) / float64(len(updateFiles)), Status: f.name + " já está atualizado."})
				continue
			}
		}
		status := "Baixando " + f.name + "..."
		var last Progress
		saved, err := u.fetch(base, address, f, want, func(part float64) {
			last = Progress{File: part, Total: (float64(i) + part) / float64(len(updateFiles)), Status: status}
			u.report(last)
		})
		if err == nil {
			state[f.name], changed = saved, true
		}
		if err != nil {
			last.Done, last.Ready = true, u.localReady()
			last.Status = "Não foi possível baixar " + f.name + ". Verifique a conexão e abra o jogo novamente."
			if last.Ready {
				last.Status = "Não foi possível atualizar " + f.name + ". Usando a cópia local."
			}
			u.report(last)
			return
		}
	}
	u.report(Progress{File: 1, Total: 1, Status: "Pronto para jogar.", Done: true, Ready: true})
}

// pageAvailable evita abrir o navegador embutido só para mostrar erro de conexão.
func pageAvailable(base string) bool {
	client := http.Client{Timeout: pageTimeout}
	res, err := client.Get(base + launcherPage)
	if err != nil {
		return false
	}
	defer res.Body.Close()
	return res.StatusCode == http.StatusOK
}
