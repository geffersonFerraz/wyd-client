package main

import (
	"bytes"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// encodeList monta um serverlist.bin com as entradas dadas, na ordem.
func encodeList(entries ...string) []byte {
	out := encodeServerList("")
	for i, text := range entries {
		encodeListEntry(out[i*len(serverListKey):(i+1)*len(serverListKey)], text)
	}
	return out
}

func listEntries(data []byte) []string {
	var got []string
	for i := 0; i < serverListSize/len(serverListKey); i++ {
		if text := decodeListEntry(data[i*len(serverListKey) : (i+1)*len(serverListKey)]); text != "" {
			got = append(got, text)
		}
	}
	return got
}

func TestNormalizeAddress(t *testing.T) {
	good := map[string]string{
		"192.168.77.26":                  "192.168.77.26",
		"  192.168.77.26 ":               "192.168.77.26",
		"192.168.77.26:8080":             "192.168.77.26:8080",
		"http://192.168.77.26/":          "192.168.77.26",
		"http://192.168.77.26:81/x":      "",
		"wyd.gefferson.com":              "wyd.gefferson.com",
		"HTTP://WYD.Gefferson.com:8080/": "",
		"http://Wyd.Gefferson.com:8080/": "wyd.gefferson.com:8080",
	}
	for in, want := range good {
		got, err := normalizeAddress(in)
		if want == "" {
			if err == nil {
				t.Errorf("%q: aceitou caminho", in)
			}
			continue
		}
		if err != nil || got != want {
			t.Errorf("%q: %q, %v", in, got, err)
		}
	}
	for _, bad := range []string{"", "a_b.com", "-a.com", "a..com", "300.1.1.1", "::1", "1.2.3.4:0", "1.2.3.4:99999", "1.2.3.4:abc"} {
		if _, err := normalizeAddress(bad); err == nil {
			t.Errorf("%q: aceitou", bad)
		}
	}
}

func TestSaveAndLoadServerAddress(t *testing.T) {
	dir := t.TempDir()
	if got := loadServerAddress(dir); got != "" {
		t.Fatalf("%q", got)
	}
	if err := saveServerAddress(dir, "10.0.0.5:8080"); err != nil {
		t.Fatal(err)
	}
	if got := loadServerAddress(dir); got != "10.0.0.5:8080" {
		t.Fatalf("%q", got)
	}
	if err := saveServerAddress(dir, "nome_ruim.com"); err == nil {
		t.Fatal("gravou endereço inválido")
	}
	if got := loadServerAddress(dir); got != "10.0.0.5:8080" {
		t.Fatalf("endereço inválido apagou o anterior: %q", got)
	}
	if err := saveServerAddress(dir, "  "); err != nil || loadServerAddress(dir) != "" {
		t.Fatal("vazio não removeu o endereço")
	}
	if err := saveServerAddress(dir, ""); err != nil {
		t.Fatalf("remover sem arquivo falhou: %v", err)
	}
}

func TestCurrentServerAddressFallsBackToServerList(t *testing.T) {
	dir := t.TempDir()
	if got := currentServerAddress(dir); got != "" {
		t.Fatalf("%q", got)
	}
	writeFile(t, filepath.Join(dir, serverListFile), encodeList("http://192.168.77.37/serv00.htm"))
	if got := currentServerAddress(dir); got != "192.168.77.37" {
		t.Fatalf("%q", got)
	}
	saveServerAddress(dir, "192.168.77.26")
	if got := currentServerAddress(dir); got != "192.168.77.26" {
		t.Fatalf("%q", got)
	}
}

func TestRewriteServerListKeepsPortsAndPath(t *testing.T) {
	in := encodeList("http://192.168.77.37/serv00.htm", "192.168.77.37", "192.168.77.37:8282", "192.168.77.37:8283")
	got := listEntries(rewriteServerList(in, "10.0.0.5:8080"))
	want := []string{"http://10.0.0.5:8080/serv00.htm", "10.0.0.5", "10.0.0.5:8282", "10.0.0.5:8283"}
	if strings.Join(got, "|") != strings.Join(want, "|") {
		t.Fatalf("got %q", got)
	}
}

func TestRewriteServerListReplacesDNSNames(t *testing.T) {
	in := encodeList("http://wyd.gefferson.com/serv00.htm", "wyd-srv1.gefferson.com", "wyd-srv2.gefferson.com:8282")
	got := listEntries(rewriteServerList(in, "10.0.0.5"))
	want := []string{"http://10.0.0.5/serv00.htm", "10.0.0.5", "10.0.0.5:8282"}
	if strings.Join(got, "|") != strings.Join(want, "|") {
		t.Fatalf("got %q", got)
	}
}

func TestRewriteServerListLeavesNonAddressesAndOriginal(t *testing.T) {
	in := encodeList("http://1.1.1.1/serv00.htm", "Servidor Um")
	before := append([]byte(nil), in...)
	got := listEntries(rewriteServerList(in, "2.2.2.2"))
	if len(got) != 2 || got[0] != "http://2.2.2.2/serv00.htm" || got[1] != "Servidor Um" {
		t.Fatalf("got %q", got)
	}
	if !bytes.Equal(in, before) {
		t.Fatal("alterou o slice de entrada")
	}
	if !bytes.Equal(rewriteServerList([]byte("curto"), "2.2.2.2"), []byte("curto")) {
		t.Fatal("mexeu em arquivo de tamanho errado")
	}
}

func TestRunUsesConfiguredAddressOverServerList(t *testing.T) {
	dir := t.TempDir()
	items := bytes.Repeat([]byte{9}, itemListSize)
	srv := serve(t, nil, items)
	address := strings.TrimPrefix(srv.URL, "http://")
	// O servidor publica a própria lista, com o IP interno dele.
	published := encodeList("http://172.30.87.13/serv00.htm", "172.30.87.21", "172.30.87.22:8282")
	srv.Config.Handler = http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		if r.URL.Path == "/serverlist.bin" {
			w.Write(published)
			return
		}
		w.Write(items)
	})
	// A lista local aponta para um servidor que não existe mais.
	writeFile(t, filepath.Join(dir, serverListFile), encodeList("http://127.0.0.1:1/serv00.htm"))
	if err := saveServerAddress(dir, address); err != nil {
		t.Fatal(err)
	}

	if p := runUpdate(dir); !p.Done || !p.Ready || p.Total != 1 {
		t.Fatalf("%+v", p)
	}
	data, err := os.ReadFile(filepath.Join(dir, serverListFile))
	if err != nil {
		t.Fatal(err)
	}
	host, _, _ := strings.Cut(address, ":")
	want := []string{"http://" + address + "/serv00.htm", host, host + ":8282"}
	if got := listEntries(data); strings.Join(got, "|") != strings.Join(want, "|") {
		t.Fatalf("got %q, want %q", got, want)
	}
}

func TestRunWithConfiguredAddressNeedsNoLocalServerList(t *testing.T) {
	dir := t.TempDir()
	items := bytes.Repeat([]byte{3}, itemListSize)
	srv := serve(t, encodeList("http://172.30.87.13/serv00.htm"), items)
	saveServerAddress(dir, strings.TrimPrefix(srv.URL, "http://"))

	if p := runUpdate(dir); !p.Done || !p.Ready {
		t.Fatalf("%+v", p)
	}
}

func TestRunPinsLocalServerListWhenDownloadFails(t *testing.T) {
	dir := t.TempDir()
	srv := serve(t, nil, nil) // Tudo dá 404.
	address := strings.TrimPrefix(srv.URL, "http://")
	writeFile(t, filepath.Join(dir, serverListFile), encodeList("http://192.168.77.37/serv00.htm", "192.168.77.37:8282"))
	writeFile(t, filepath.Join(dir, itemListFile), bytes.Repeat([]byte{1}, itemListSize))
	saveServerAddress(dir, address)

	if p := runUpdate(dir); !p.Done || !p.Ready || !strings.Contains(p.Status, "cópia local") {
		t.Fatalf("%+v", p)
	}
	data, _ := os.ReadFile(filepath.Join(dir, serverListFile))
	host, _, _ := strings.Cut(address, ":")
	if got := listEntries(data); len(got) != 2 || got[0] != "http://"+address+"/serv00.htm" || got[1] != host+":8282" {
		t.Fatalf("lista local não foi ajustada: %q", got)
	}
}

func TestLauncherBasePrefersConfiguredAddress(t *testing.T) {
	dir := t.TempDir()
	if _, err := launcherBase(dir); err == nil {
		t.Fatal("sem nenhuma fonte deveria falhar")
	}
	writeFile(t, filepath.Join(dir, serverListFile), encodeList("http://192.168.77.37/serv00.htm"))
	if got, err := launcherBase(dir); err != nil || got != "http://192.168.77.37" {
		t.Fatalf("%q, %v", got, err)
	}
	saveServerAddress(dir, "192.168.77.26:81")
	if got, err := launcherBase(dir); err != nil || got != "http://192.168.77.26:81" {
		t.Fatalf("%q, %v", got, err)
	}
}
