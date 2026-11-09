package main

import (
	"bytes"
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
	got, err := serverBase(encodeServerList("http://192.168.77.191:8080/serv00.htm"))
	if err != nil || got != "http://192.168.77.191:8080" {
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
