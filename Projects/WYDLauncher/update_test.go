package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"net/http/httptest"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func encodeServerList(address string) []byte {
	out := make([]byte, serverListSize)
	copy(out, address)
	for i := range out {
		out[i] += serverListKey[len(serverListKey)-1-i%len(serverListKey)]
	}
	return out
}

func serve(t *testing.T, list, items []byte) *httptest.Server {
	t.Helper()
	srv := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		switch {
		case r.URL.Path == "/serverlist.bin" && list != nil:
			w.Write(list)
		case r.URL.Path == "/itemlist.bin" && items != nil:
			w.Write(items)
		default:
			http.NotFound(w, r)
		}
	}))
	t.Cleanup(srv.Close)
	return srv
}

func runUpdate(dir string) Progress {
	var last Progress
	newUpdater(dir, func(p Progress) { last = p }).run()
	return last
}

func writeFile(t *testing.T, path string, data []byte) {
	t.Helper()
	if err := os.WriteFile(path, data, 0o644); err != nil {
		t.Fatal(err)
	}
}

func TestServerBaseKeepsPortAndDropsPath(t *testing.T) {
	got, err := serverBase(encodeServerList("http://192.168.77.26:8081/serv00.htm"))
	if err != nil || got != "http://192.168.77.26:8081" {
		t.Fatalf("%q, %v", got, err)
	}
}

func TestServerBaseRejectsListWithoutURL(t *testing.T) {
	if _, err := serverBase(encodeServerList("")); err == nil {
		t.Fatal("aceitou lista sem endereço")
	}
	if _, err := serverBase([]byte("<html>")); err == nil {
		t.Fatal("aceitou arquivo curto")
	}
}

func TestRunReplacesBothFilesAndKeepsReleaseSpelling(t *testing.T) {
	dir := t.TempDir()
	items := bytes.Repeat([]byte{7}, itemListSize)
	srv := serve(t, nil, items)
	list := encodeServerList(srv.URL + "/serv00.htm")
	srv.Config.Handler = http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		if r.URL.Path == "/serverlist.bin" {
			w.Write(list)
			return
		}
		w.Write(items)
	})
	writeFile(t, filepath.Join(dir, serverListFile), list)
	writeFile(t, filepath.Join(dir, "Itemlist.bin"), []byte("velho"))

	if p := runUpdate(dir); !p.Done || !p.Ready || p.Total != 1 {
		t.Fatalf("%+v", p)
	}
	got, err := os.ReadFile(filepath.Join(dir, "Itemlist.bin"))
	if err != nil || !bytes.Equal(got, items) {
		t.Fatalf("itemlist não foi trocado: %v, %d bytes", err, len(got))
	}
	// Listar em vez de os.Stat: no Windows o Stat não diferencia maiúsculas.
	entries, err := os.ReadDir(dir)
	if err != nil {
		t.Fatal(err)
	}
	copies := 0
	for _, e := range entries {
		if strings.EqualFold(e.Name(), itemListFile) {
			copies++
		}
	}
	if copies != 1 {
		t.Fatalf("%d cópias do itemlist", copies)
	}
}

func TestRunKeepsLocalFilesWhenDownloadIsBad(t *testing.T) {
	dir := t.TempDir()
	srv := serve(t, []byte("<html>erro</html>"), nil)
	list := encodeServerList(srv.URL + "/serv00.htm")
	items := bytes.Repeat([]byte{1}, itemListSize)
	writeFile(t, filepath.Join(dir, serverListFile), list)
	writeFile(t, filepath.Join(dir, itemListFile), items)

	p := runUpdate(dir)
	if !p.Done || !p.Ready || !strings.Contains(p.Status, "cópia local") {
		t.Fatalf("%+v", p)
	}
	if got, _ := os.ReadFile(filepath.Join(dir, serverListFile)); !bytes.Equal(got, list) {
		t.Fatal("download inválido sobrescreveu o arquivo local")
	}
}

func TestRunIsNotReadyWithoutItemList(t *testing.T) {
	dir := t.TempDir()
	srv := serve(t, nil, nil)
	writeFile(t, filepath.Join(dir, serverListFile), encodeServerList(srv.URL+"/serv00.htm"))

	if p := runUpdate(dir); !p.Done || p.Ready {
		t.Fatalf("%+v", p)
	}
}

func TestRunIsNotReadyWithoutServerList(t *testing.T) {
	if p := runUpdate(t.TempDir()); !p.Done || p.Ready {
		t.Fatalf("%+v", p)
	}
}

// manifestServer serve os dois arquivos e o manifesto deles, e conta os downloads.
type manifestServer struct {
	*httptest.Server
	list, items []byte
	manifest    string // "" = calculado dos arquivos
	downloads   map[string]int
}

func serveManifest(t *testing.T, items []byte) *manifestServer {
	t.Helper()
	m := &manifestServer{items: items, downloads: map[string]int{}}
	m.Server = httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		switch r.URL.Path {
		case manifestPath:
			body := m.manifest
			if body == "" {
				body = fmt.Sprintf(`{"version":1,"files":[{"name":"serverlist.bin","path":"/serverlist.bin","size":%d,"sha256":%q},{"name":"ItemList.bin","path":"/itemlist.bin","size":%d,"sha256":%q}]}`,
					len(m.list), hashOf(m.list), len(m.items), hashOf(m.items))
			}
			w.Write([]byte(body))
		case "/serverlist.bin":
			m.downloads[r.URL.Path]++
			w.Write(m.list)
		case "/itemlist.bin":
			m.downloads[r.URL.Path]++
			w.Write(m.items)
		default:
			http.NotFound(w, r)
		}
	}))
	t.Cleanup(m.Close)
	m.list = encodeServerList(m.URL + "/serv00.htm")
	return m
}

func TestRunSkipsFilesThatMatchTheManifest(t *testing.T) {
	dir := t.TempDir()
	srv := serveManifest(t, bytes.Repeat([]byte{7}, itemListSize))
	writeFile(t, filepath.Join(dir, serverListFile), srv.list)
	writeFile(t, filepath.Join(dir, itemListFile), srv.items)

	if p := runUpdate(dir); !p.Done || !p.Ready || p.Total != 1 {
		t.Fatalf("%+v", p)
	}
	if len(srv.downloads) != 0 {
		t.Fatalf("baixou arquivos iguais: %v", srv.downloads)
	}
}

func TestRunDownloadsOnlyTheChangedFile(t *testing.T) {
	dir := t.TempDir()
	srv := serveManifest(t, bytes.Repeat([]byte{7}, itemListSize))
	writeFile(t, filepath.Join(dir, serverListFile), srv.list)
	writeFile(t, filepath.Join(dir, itemListFile), bytes.Repeat([]byte{1}, itemListSize))

	if p := runUpdate(dir); !p.Ready {
		t.Fatalf("%+v", p)
	}
	if got, _ := os.ReadFile(filepath.Join(dir, itemListFile)); !bytes.Equal(got, srv.items) {
		t.Fatal("itemlist não foi atualizado")
	}
	if srv.downloads["/itemlist.bin"] != 1 || srv.downloads["/serverlist.bin"] != 0 {
		t.Fatalf("downloads: %v", srv.downloads)
	}
	// Outra abertura não baixa nada; um itemlist novo no servidor, só ele.
	runUpdate(dir)
	srv.items = bytes.Repeat([]byte{9}, itemListSize)
	runUpdate(dir)
	if srv.downloads["/itemlist.bin"] != 2 || srv.downloads["/serverlist.bin"] != 0 {
		t.Fatalf("downloads: %v", srv.downloads)
	}
}

// Com o IP das Configurações, o serverlist.bin em disco nunca tem o hash do
// manifesto: o estado salvo é que evita baixá-lo a cada abertura.
func TestRunSkipsPinnedServerListUntilAddressChanges(t *testing.T) {
	dir := t.TempDir()
	srv := serveManifest(t, bytes.Repeat([]byte{7}, itemListSize))
	address := strings.TrimPrefix(srv.URL, "http://")
	srv.list = encodeServerList("http://10.9.9.9/serv00.htm")
	writeFile(t, filepath.Join(dir, itemListFile), srv.items)
	if err := saveServerAddress(dir, address); err != nil {
		t.Fatal(err)
	}

	for range 3 {
		if p := runUpdate(dir); !p.Ready {
			t.Fatalf("%+v", p)
		}
	}
	if srv.downloads["/serverlist.bin"] != 1 {
		t.Fatalf("downloads: %v", srv.downloads)
	}
	local, _ := os.ReadFile(filepath.Join(dir, serverListFile))
	if base, err := serverBase(local); err != nil || base != srv.URL {
		t.Fatalf("serverlist sem o endereço das Configurações: %q, %v", base, err)
	}

	// Sem o endereço, o arquivo do servidor volta. O painel segue acessível
	// nesta abertura porque a lista local ainda aponta para ele.
	if err := saveServerAddress(dir, ""); err != nil {
		t.Fatal(err)
	}
	runUpdate(dir)
	if srv.downloads["/serverlist.bin"] != 2 {
		t.Fatalf("não baixou de novo sem o endereço: %v", srv.downloads)
	}
	if local, _ = os.ReadFile(filepath.Join(dir, serverListFile)); !bytes.Equal(local, srv.list) {
		t.Fatal("serverlist não voltou ao do servidor")
	}
}

func TestRunRejectsDownloadThatDiffersFromManifest(t *testing.T) {
	dir := t.TempDir()
	srv := serveManifest(t, bytes.Repeat([]byte{7}, itemListSize))
	old := bytes.Repeat([]byte{1}, itemListSize)
	writeFile(t, filepath.Join(dir, serverListFile), srv.list)
	writeFile(t, filepath.Join(dir, itemListFile), old)
	srv.manifest = fmt.Sprintf(`{"version":1,"files":[{"name":"ItemList.bin","path":"/itemlist.bin","size":%d,"sha256":%q}]}`,
		itemListSize, hashOf([]byte("outro conteúdo")))

	p := runUpdate(dir)
	if !p.Done || !p.Ready || !strings.Contains(p.Status, "cópia local") {
		t.Fatalf("%+v", p)
	}
	if got, _ := os.ReadFile(filepath.Join(dir, itemListFile)); !bytes.Equal(got, old) {
		t.Fatal("download com hash errado substituiu o arquivo local")
	}
}

func TestRunDownloadsEverythingWhenManifestIsUnusable(t *testing.T) {
	for _, manifest := range []string{"<html>", `{"version":2,"files":[]}`, `{"version":1,"files":[{"path":"/itemlist.bin","sha256":"curto"}]}`} {
		dir := t.TempDir()
		srv := serveManifest(t, bytes.Repeat([]byte{7}, itemListSize))
		srv.manifest = manifest
		writeFile(t, filepath.Join(dir, serverListFile), srv.list)
		writeFile(t, filepath.Join(dir, itemListFile), srv.items)

		if p := runUpdate(dir); !p.Ready {
			t.Fatalf("%q: %+v", manifest, p)
		}
		if srv.downloads["/itemlist.bin"] != 1 || srv.downloads["/serverlist.bin"] != 1 {
			t.Fatalf("%q: downloads %v", manifest, srv.downloads)
		}
	}
}

func TestStateFileIsValidJSON(t *testing.T) {
	dir := t.TempDir()
	srv := serveManifest(t, bytes.Repeat([]byte{7}, itemListSize))
	writeFile(t, filepath.Join(dir, serverListFile), srv.list)
	runUpdate(dir)

	data, err := os.ReadFile(filepath.Join(dir, stateFile))
	if err != nil {
		t.Fatal(err)
	}
	var state map[string]fileState
	if err := json.Unmarshal(data, &state); err != nil {
		t.Fatal(err)
	}
	if got := state[itemListFile]; got.Remote != hashOf(srv.items) || got.Local != got.Remote || got.Address != "" {
		t.Fatalf("%+v", state)
	}
}
