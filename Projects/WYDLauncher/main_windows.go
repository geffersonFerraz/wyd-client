// JogarWYD.exe: launcher do cliente (atualização, página do servidor e opções),
// igual no Windows e no Linux.
// No Windows é aberto direto. No Linux, o WYD-Linux abre pelo Wine com --configurar.
package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
)

//go:generate x86_64-w64-mingw32-windres launcher.rc -O coff -o rsrc_windows_amd64.syso

const (
	exitPlay  = 0
	exitClose = 1
)

func main() {
	os.Exit(run(os.Args[1:]))
}

func run(args []string) int {
	configureOnly := len(args) > 0 && args[0] == "--configurar"
	if configureOnly {
		args = args[1:]
	}
	if err := openAndPlay(args, configureOnly); err != nil {
		if err != errClosed {
			messageBox(err.Error())
		}
		return exitClose
	}
	return exitPlay
}

func openAndPlay(args []string, configureOnly bool) error {
	exe, err := os.Executable()
	if err != nil {
		return fmt.Errorf("não foi possível localizar o programa: %w", err)
	}
	dir := filepath.Dir(exe)
	game := filepath.Join(dir, "WYD.exe")
	for _, file := range []string{game, filepath.Join(dir, "D3DX9_43.dll"), filepath.Join(dir, "D3DCompiler_43.dll")} {
		if _, err := os.Stat(file); err != nil {
			return fmt.Errorf("arquivo não encontrado: %s\nExtraia o jogo novamente, mantendo todas as pastas.", file)
		}
	}
	path := filepath.Join(dir, configFile)
	cfg, err := loadConfig(path)
	if err != nil {
		return err
	}
	if err := runLauncher(dir, path, cfg); err != nil {
		return err
	}
	if configureOnly {
		return nil
	}
	cmd := exec.Command(game, args...)
	cmd.Dir = dir
	if err := cmd.Start(); err != nil {
		return fmt.Errorf("não foi possível abrir o jogo: %w", err)
	}
	return cmd.Process.Release()
}
