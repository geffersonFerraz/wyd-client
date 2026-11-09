package main

import (
	"reflect"
	"testing"
)

func TestParseLibrariesKeepsOnly64Bit(t *testing.T) {
	out := "2 libs found in cache\n" +
		"\tlibX11.so.6 (libc6,x86-64) => /lib/x86_64-linux-gnu/libX11.so.6\n" +
		"\tlibGL.so.1 (libc6) => /lib/i386-linux-gnu/libGL.so.1\n"
	want := map[string]bool{"libX11.so.6": true}
	if got := parseLibraries(out); !reflect.DeepEqual(got, want) {
		t.Fatalf("got %v, want %v", got, want)
	}
}

func TestSelectMissing(t *testing.T) {
	items := []check{
		{name: "som", libs: []string{"libpulse.so.0", "libasound.so.2"}},
		{name: "gl", libs: []string{"libGL.so.1"}, required: true},
		{name: "mp3", plugins: []string{"libgstmpg123.so"}},
		{name: "base", plugins: []string{"libgstplayback.so"}},
	}
	libs := map[string]bool{"libasound.so.2": true}
	missing := selectMissing(items, libs, func(p string) bool { return p == "libgstplayback.so" })
	if got := namesOf(missing); got != "gl, mp3" {
		t.Fatalf("got %q", got)
	}
	if !anyRequired(missing) {
		t.Fatal("gl é obrigatório")
	}
}

func TestPackagesOfRemovesDuplicates(t *testing.T) {
	got := packagesOf([]check{{pkg: "a"}, {pkg: "b"}, {pkg: "a"}})
	if !reflect.DeepEqual(got, []string{"a", "b"}) {
		t.Fatalf("got %v", got)
	}
}
