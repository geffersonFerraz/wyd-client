package main

import (
	"fmt"
	"os"
	"os/exec"
	"sync"
)

func hasTool(name string) bool {
	_, err := exec.LookPath(name)
	return err == nil
}

func showError(text string) {
	fmt.Fprintln(os.Stderr, "WYD:", text)
	switch {
	case hasTool("zenity"):
		exec.Command("zenity", "--error", "--no-markup", "--title", appTitle, "--text", text).Run()
	case hasTool("kdialog"):
		exec.Command("kdialog", "--title", appTitle, "--error", text).Run()
	}
}

// askYesNo responde não quando não há como perguntar.
func askYesNo(text string) bool {
	switch {
	case hasTool("zenity"):
		return exec.Command("zenity", "--question", "--no-markup", "--title", appTitle, "--text", text).Run() == nil
	case hasTool("kdialog"):
		return exec.Command("kdialog", "--title", appTitle, "--yesno", text).Run() == nil
	}
	fmt.Fprintln(os.Stderr, "WYD:", text)
	return false
}

// showProgress abre uma janela de espera e devolve a função que a fecha.
func showProgress(text string) (stop func()) {
	fmt.Fprintln(os.Stderr, "WYD:", text)
	if !hasTool("zenity") {
		return func() {}
	}
	cmd := exec.Command("zenity", "--progress", "--pulsate", "--no-cancel", "--title", appTitle, "--text", text)
	// A janela fecharia ao ver o fim da entrada: o pipe fica aberto até stop.
	stdin, err := cmd.StdinPipe()
	if err != nil || cmd.Start() != nil {
		return func() {}
	}
	var once sync.Once
	return func() {
		once.Do(func() {
			stdin.Close()
			cmd.Process.Kill()
			cmd.Wait()
		})
	}
}
