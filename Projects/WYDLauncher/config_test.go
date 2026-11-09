package main

import (
	"bytes"
	"os"
	"path/filepath"
	"testing"
)

// Bytes reais do Config.bin da release, com 2 bytes além da estrutura.
var realConfig = []byte{
	0xea, 0x29, 1, 0, 2, 0, 1, 0, 1, 0, 0xff, 0xff, 0, 0, 2, 0,
	0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 3, 0,
}

func TestSettingsFromRealFile(t *testing.T) {
	got := parseConfig(realConfig).settings()
	want := Settings{Resolution: 1, Sound: 1, Music: 1, Bright: 0, SystemCursor: true, CompressedTextures: false}
	if got != want {
		t.Fatalf("got %+v, want %+v", got, want)
	}
}

func TestApplyWithoutChangesKeepsFile(t *testing.T) {
	c := parseConfig(realConfig)
	c.apply(c.settings())
	if !bytes.Equal(c.raw, realConfig) {
		t.Fatalf("got % x", c.raw)
	}
}

func TestApplyChangesOnlyKnownFields(t *testing.T) {
	c := parseConfig(realConfig)
	s := c.settings()
	s.Resolution, s.Music, s.Fullscreen = 5, 70, true
	c.apply(s)
	if c.get(cfgResolution) != 5 || c.get(cfgMusic) != 70 || c.get(cfgWindowed) != 0 {
		t.Fatalf("got % x", c.raw)
	}
	if c.get(1) != 2 || c.get(4) != -1 || c.get(13) != 3 || !bytes.Equal(c.raw[:2], realConfig[:2]) || !bytes.Equal(c.raw[30:], realConfig[30:]) {
		t.Fatalf("campo preservado mudou: % x", c.raw)
	}
}

func TestOutOfRangeValuesAreNormalized(t *testing.T) {
	c := defaultConfig()
	c.set(cfgResolution, 9)
	c.set(cfgMusic, 500)
	c.set(cfgSound, -3)
	s := c.settings()
	if s.Resolution != 1 || s.Music != 100 || s.Sound != 0 {
		t.Fatalf("got %+v", s)
	}
}

func TestShortFileUsesClientDefaultsForTheRest(t *testing.T) {
	c := parseConfig(realConfig[:4])
	if len(c.raw) != configSize || c.get(cfgResolution) != 1 || c.get(cfgBright) != 57 {
		t.Fatalf("got % x", c.raw)
	}
}

func TestSaveAndLoad(t *testing.T) {
	path := filepath.Join(t.TempDir(), configFile)
	c, err := loadConfig(path)
	if err != nil || c.get(cfgBright) != 57 {
		t.Fatalf("padrão: %v %+v", err, c)
	}
	c.set(cfgMusic, 42)
	if err := c.save(path); err != nil {
		t.Fatal(err)
	}
	loaded, err := loadConfig(path)
	if err != nil || loaded.get(cfgMusic) != 42 {
		t.Fatalf("%v %+v", err, loaded)
	}
	if _, err := os.Stat(path + ".tmp"); !os.IsNotExist(err) {
		t.Fatal("arquivo temporário ficou para trás")
	}
}
