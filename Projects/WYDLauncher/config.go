package main

import (
	"encoding/binary"
	"errors"
	"fmt"
	"io/fs"
	"os"
)

// Config.bin: short Version + short Config[14], lido uma vez ao abrir o cliente
// (NewApp::Initialize). O cliente não grava este arquivo. Os campos que a tela
// não edita são preservados, inclusive bytes além do tamanho da estrutura.
const (
	configFile = "Config.bin"
	configSize = 30

	cfgResolution  = 0
	cfgSound       = 2
	cfgMusic       = 3
	cfgBright      = 5
	cfgCursor      = 6
	cfgWindowed    = 8
	cfgCameraRot   = 10
	cfgNoDXT       = 11
	cfgChatAlways  = 12
	cursorDrawn    = 1
	cursorSystem   = 2
	maxPercent     = 100
	configBaseSlot = 2 // Config[0] começa depois de Version.
)

// Resolution é uma entrada que o cliente sabe usar. O ID 1 pede 600x480 ao
// cliente, que não acha esse modo e cai para 800x600, o tamanho real do layout.
type Resolution struct {
	ID            int
	Width, Height uint32
}

var resolutions = []Resolution{
	{1, 800, 600},
	{2, 1024, 768},
	{3, 1280, 1024},
	{4, 1366, 768},
	{5, 1920, 1080},
}

// Settings são as opções que a tela mostra.
type Settings struct {
	Resolution         int
	Fullscreen         bool
	Bright             int
	Sound              int
	Music              int
	SystemCursor       bool
	CameraRotate       bool
	CompressedTextures bool
	ChatAlwaysOn       bool
}

// config guarda o conteúdo do arquivo.
type config struct {
	raw []byte
}

// defaultConfig repete os valores que o cliente usa quando o arquivo não existe.
func defaultConfig() *config {
	values := []int16{7000, 2, 2, 0, 0, -1, 57, 2, 1, 1, 1, 0, 0, 0, 1}
	raw := make([]byte, configSize)
	for i, v := range values {
		binary.LittleEndian.PutUint16(raw[2*i:], uint16(v))
	}
	return &config{raw: raw}
}

func parseConfig(data []byte) *config {
	c := defaultConfig()
	if len(data) >= configSize {
		c.raw = append([]byte(nil), data...)
	} else {
		copy(c.raw, data)
	}
	return c
}

func loadConfig(path string) (*config, error) {
	data, err := os.ReadFile(path)
	if errors.Is(err, fs.ErrNotExist) {
		return defaultConfig(), nil
	}
	if err != nil {
		return nil, fmt.Errorf("não foi possível ler %s: %w", path, err)
	}
	return parseConfig(data), nil
}

func (c *config) save(path string) error {
	return replaceFile(path, c.raw)
}

func (c *config) get(index int) int {
	return int(int16(binary.LittleEndian.Uint16(c.raw[configBaseSlot+2*index:])))
}

func (c *config) set(index, value int) {
	binary.LittleEndian.PutUint16(c.raw[configBaseSlot+2*index:], uint16(int16(value)))
}

func clampPercent(value int) int {
	return max(0, min(value, maxPercent))
}

func validResolution(id int) int {
	for _, r := range resolutions {
		if r.ID == id {
			return id
		}
	}
	return resolutions[0].ID
}

func (c *config) settings() Settings {
	return Settings{
		Resolution:         validResolution(c.get(cfgResolution)),
		Fullscreen:         c.get(cfgWindowed) == 0,
		Bright:             clampPercent(c.get(cfgBright)),
		Sound:              clampPercent(c.get(cfgSound)),
		Music:              clampPercent(c.get(cfgMusic)),
		SystemCursor:       c.get(cfgCursor) != cursorDrawn,
		CameraRotate:       c.get(cfgCameraRot) > 0,
		CompressedTextures: c.get(cfgNoDXT) <= 0,
		ChatAlwaysOn:       c.get(cfgChatAlways) == 1,
	}
}

func boolValue(b bool) int {
	if b {
		return 1
	}
	return 0
}

func (c *config) apply(s Settings) {
	cursor := cursorDrawn
	if s.SystemCursor {
		cursor = cursorSystem
	}
	c.set(cfgResolution, validResolution(s.Resolution))
	c.set(cfgWindowed, boolValue(!s.Fullscreen))
	c.set(cfgBright, clampPercent(s.Bright))
	c.set(cfgSound, clampPercent(s.Sound))
	c.set(cfgMusic, clampPercent(s.Music))
	c.set(cfgCursor, cursor)
	c.set(cfgCameraRot, boolValue(s.CameraRotate))
	c.set(cfgNoDXT, boolValue(!s.CompressedTextures))
	c.set(cfgChatAlways, boolValue(s.ChatAlwaysOn))
}
