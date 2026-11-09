package main

import (
	"errors"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

// check é um item do sistema de que o Wine precisa. Basta um dos arquivos existir.
type check struct {
	name     string
	libs     []string
	plugins  []string
	pkg      string
	required bool
}

// A lista vem das bibliotecas que o Wine 10.0 embutido carrega. Os nomes de
// pacote valem para Ubuntu 24.04 ou mais novo.
var checks = []check{
	{name: "X11", libs: []string{"libX11.so.6"}, pkg: "libx11-6", required: true},
	{name: "X11 Ext", libs: []string{"libXext.so.6"}, pkg: "libxext6", required: true},
	{name: "OpenGL", libs: []string{"libGL.so.1"}, pkg: "libgl1", required: true},
	{name: "FreeType", libs: []string{"libfreetype.so.6"}, pkg: "libfreetype6", required: true},

	{name: "Fontconfig", libs: []string{"libfontconfig.so.1"}, pkg: "libfontconfig1"},
	{name: "XRender", libs: []string{"libXrender.so.1"}, pkg: "libxrender1"},
	{name: "XRandR", libs: []string{"libXrandr.so.2"}, pkg: "libxrandr2"},
	{name: "XCursor", libs: []string{"libXcursor.so.1"}, pkg: "libxcursor1"},
	{name: "XInput", libs: []string{"libXi.so.6"}, pkg: "libxi6"},
	{name: "XFixes", libs: []string{"libXfixes.so.3"}, pkg: "libxfixes3"},
	{name: "som", libs: []string{"libpulse.so.0", "libasound.so.2"}, pkg: "libpulse0"},
	{name: "GStreamer", libs: []string{"libgstreamer-1.0.so.0"}, pkg: "libgstreamer1.0-0"},
	{name: "GStreamer base", libs: []string{"libgstaudio-1.0.so.0"}, pkg: "libgstreamer-plugins-base1.0-0"},
	{name: "reprodução de mídia", plugins: []string{"libgstplayback.so"}, pkg: "gstreamer1.0-plugins-base"},
	{name: "música em MP3", plugins: []string{"libgstmpg123.so"}, pkg: "gstreamer1.0-plugins-good"},
}

var pluginDirs = []string{
	"/usr/lib/x86_64-linux-gnu/gstreamer-1.0",
	"/usr/lib64/gstreamer-1.0",
	"/usr/lib/gstreamer-1.0",
}

// parseLibraries lê a saída de ldconfig -p e guarda só as bibliotecas de 64 bits.
func parseLibraries(output string) map[string]bool {
	libs := map[string]bool{}
	for _, line := range strings.Split(output, "\n") {
		fields := strings.Fields(line)
		if len(fields) >= 2 && strings.Contains(line, "x86-64") {
			libs[fields[0]] = true
		}
	}
	return libs
}

func listLibraries() (map[string]bool, error) {
	path, err := exec.LookPath("ldconfig")
	if err != nil {
		path = "/sbin/ldconfig"
	}
	out, err := exec.Command(path, "-p").Output()
	if err != nil {
		return nil, fmt.Errorf("não foi possível listar as bibliotecas do sistema: %w", err)
	}
	return parseLibraries(string(out)), nil
}

func pluginExists(name string) bool {
	for _, dir := range pluginDirs {
		if _, err := os.Stat(filepath.Join(dir, name)); err == nil {
			return true
		}
	}
	return false
}

func selectMissing(items []check, libs map[string]bool, hasPlugin func(string) bool) []check {
	var missing []check
	for _, c := range items {
		found := false
		for _, lib := range c.libs {
			found = found || libs[lib]
		}
		for _, plugin := range c.plugins {
			found = found || hasPlugin(plugin)
		}
		if !found {
			missing = append(missing, c)
		}
	}
	return missing
}

func findMissing() ([]check, error) {
	libs, err := listLibraries()
	if err != nil {
		return nil, err
	}
	return selectMissing(checks, libs, pluginExists), nil
}

func anyRequired(items []check) bool {
	for _, c := range items {
		if c.required {
			return true
		}
	}
	return false
}

func packagesOf(items []check) []string {
	var pkgs []string
	seen := map[string]bool{}
	for _, c := range items {
		if !seen[c.pkg] {
			seen[c.pkg] = true
			pkgs = append(pkgs, c.pkg)
		}
	}
	return pkgs
}

func namesOf(items []check) string {
	names := make([]string, len(items))
	for i, c := range items {
		names[i] = c.name
	}
	return strings.Join(names, ", ")
}

func installCommand(pkgs []string) string {
	return "apt-get update && apt-get install -y --no-install-recommends " + strings.Join(pkgs, " ")
}

func canInstall() bool {
	for _, tool := range []string{"apt-get", "pkexec"} {
		if _, err := exec.LookPath(tool); err != nil {
			return false
		}
	}
	return true
}

func install(pkgs []string) error {
	stop := showProgress("Instalando os componentes que faltam.")
	defer stop()
	return exec.Command("pkexec", "sh", "-c", installCommand(pkgs)).Run()
}

// resolveDependencies só bloqueia o jogo quando falta item obrigatório. Sem os
// opcionais o jogo abre com menos recursos, e a recusa fica salva para não perguntar de novo.
func (l *launcher) resolveDependencies() error {
	missing, err := findMissing()
	if err != nil {
		return nil // Sem ldconfig não há como validar: deixa o Wine tentar.
	}
	if len(missing) == 0 {
		return nil
	}
	required := anyRequired(missing)
	declined := filepath.Join(l.dataDir, "opcionais-recusados")
	if !required && readFile(declined) == namesOf(missing) {
		return nil
	}

	pkgs := packagesOf(missing)
	manual := "sudo sh -c '" + installCommand(pkgs) + "'"
	if !canInstall() {
		if required {
			return fmt.Errorf("faltam componentes: %s.\nInstale com:\n%s", namesOf(missing), manual)
		}
		os.WriteFile(declined, []byte(namesOf(missing)), 0o644)
		return nil
	}

	question := "Faltam componentes no sistema: " + namesOf(missing) + ".\nInstalar agora? A senha de administrador será pedida."
	if !required {
		question += "\nSem eles o jogo abre, mas pode ficar sem música ou som."
	}
	if !askYesNo(question) {
		if required {
			return errors.New("o jogo não pode abrir sem: " + namesOf(missing))
		}
		os.WriteFile(declined, []byte(namesOf(missing)), 0o644)
		return nil
	}

	install(pkgs)
	if remaining, err := findMissing(); err == nil && anyRequired(remaining) {
		return fmt.Errorf("ainda faltam componentes: %s.\nInstale com:\n%s", namesOf(remaining), manual)
	}
	return nil
}

func readFile(path string) string {
	data, err := os.ReadFile(path)
	if err != nil {
		return ""
	}
	return string(data)
}
