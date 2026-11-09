// Lançador do cliente no Linux: valida o sistema, prepara o Wine embutido, abre o
// launcher (JogarWYD.exe) e depois o WYD.exe.
// Fica na pasta do jogo, ao lado de WYD.exe e wine/. Compile com ./wyd-linux.sh bundle.
package main

import (
	"errors"
	"fmt"
	"io"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"
)

const (
	appTitle     = "WYD"
	dllOverrides = "d3dx9_43=n,b;d3dcompiler_43=n,b;mscoree,mshtml="
	crashMarker  = "[wyd-crash]"

	settingsProgram = "JogarWYD.exe"
	settingsClosed  = 1 // Código de saída do JogarWYD.exe quando o jogador cancela.
	bootTimeout     = 10 * time.Minute

	// O launcher precisa do mshtml, que o resto do jogo deixa desligado.
	browserOverrides = "mscoree="
	geckoInstaller   = `wine\share\wine\gecko\wine-gecko-2.47.4-x86_64.msi`
	geckoMarker      = ".wyd-gecko"
)

type launcher struct {
	gameDir string
	dataDir string
	prefix  string
	wineDir string
}

func main() {
	os.Exit(run(os.Args[1:]))
}

func run(args []string) int {
	l, err := newLauncher()
	if err != nil {
		showError(err.Error())
		return 1
	}
	if len(args) > 0 && args[0] == "--check" {
		return l.printCheck()
	}
	if err := l.prepare(); err != nil {
		showError(err.Error())
		return 1
	}
	if len(args) > 0 && args[0] == "--jogar" {
		return l.play(args[1:])
	}
	play, err := l.configure()
	if err != nil {
		showError(err.Error())
		return 1
	}
	if !play {
		return 0
	}
	return l.play(args)
}

func newLauncher() (*launcher, error) {
	exe, err := os.Executable()
	if err != nil {
		return nil, fmt.Errorf("não foi possível localizar o lançador: %w", err)
	}
	if exe, err = filepath.EvalSymlinks(exe); err != nil {
		return nil, fmt.Errorf("não foi possível localizar o lançador: %w", err)
	}
	dataDir, err := dataDirectory()
	if err != nil {
		return nil, err
	}
	gameDir := filepath.Dir(exe)
	prefix := os.Getenv("WINEPREFIX")
	if prefix == "" {
		prefix = filepath.Join(dataDir, "prefix-embutido")
	}
	return &launcher{
		gameDir: gameDir,
		dataDir: dataDir,
		prefix:  prefix,
		wineDir: filepath.Join(gameDir, "wine", "bin"),
	}, nil
}

func dataDirectory() (string, error) {
	base := os.Getenv("XDG_DATA_HOME")
	if base == "" {
		home, err := os.UserHomeDir()
		if err != nil {
			return "", fmt.Errorf("não foi possível localizar a pasta pessoal: %w", err)
		}
		base = filepath.Join(home, ".local", "share")
	}
	dir := filepath.Join(base, "wyd-linux")
	if err := os.MkdirAll(dir, 0o755); err != nil {
		return "", fmt.Errorf("não foi possível criar %s: %w", dir, err)
	}
	return dir, nil
}

// checkInstall confirma os arquivos que vêm na própria release.
func (l *launcher) checkInstall() error {
	for _, file := range []string{
		filepath.Join(l.gameDir, "WYD.exe"),
		filepath.Join(l.gameDir, settingsProgram),
		filepath.Join(l.wineDir, "wine"),
		filepath.Join(l.wineDir, "wineserver"),
	} {
		if _, err := os.Stat(file); err != nil {
			return fmt.Errorf("arquivo não encontrado: %s\nExtraia o jogo novamente, mantendo todas as pastas.", file)
		}
	}
	if os.Getenv("DISPLAY") == "" {
		return errors.New("nenhuma sessão gráfica encontrada (DISPLAY vazio). Abra o jogo de dentro do ambiente gráfico.")
	}
	return nil
}

func (l *launcher) printCheck() int {
	code := 0
	if err := l.checkInstall(); err != nil {
		fmt.Println("ERRO:", err)
		code = 1
	}
	missing, err := findMissing()
	if err != nil {
		fmt.Println("AVISO:", err)
	}
	for _, c := range missing {
		kind := "opcional"
		if c.required {
			kind = "obrigatório"
			code = 1
		}
		fmt.Printf("FALTA (%s): %s, pacote %s\n", kind, c.name, c.pkg)
	}
	if code == 0 && len(missing) == 0 {
		fmt.Println("OK: tudo o que o jogo precisa está presente.")
	}
	return code
}

func (l *launcher) prepare() error {
	if err := l.checkInstall(); err != nil {
		return err
	}
	if err := l.resolveDependencies(); err != nil {
		return err
	}
	if err := l.ensurePrefix(); err != nil {
		return err
	}
	l.ensureGecko()
	return nil
}

func (l *launcher) env() []string {
	var env []string
	for _, kv := range os.Environ() {
		// Variáveis de outro Wine instalado fariam o embutido carregar arquivos errados.
		switch strings.SplitN(kv, "=", 2)[0] {
		case "WINEPREFIX", "WINELOADER", "WINESERVER", "WINEDLLPATH", "WINEDLLOVERRIDES", "WINE":
			continue
		}
		env = append(env, kv)
	}
	env = append(env,
		"WINEPREFIX="+l.prefix,
		"WINEDLLOVERRIDES="+dllOverrides,
		"PATH="+l.wineDir+string(os.PathListSeparator)+os.Getenv("PATH"),
	)
	if os.Getenv("WINEDEBUG") == "" {
		env = append(env, "WINEDEBUG=fixme-all")
	}
	return env
}

func (l *launcher) wine(out io.Writer, args ...string) *exec.Cmd {
	cmd := exec.Command(filepath.Join(l.wineDir, "wine"), args...)
	cmd.Dir = l.gameDir
	cmd.Env = l.env()
	cmd.Stdout = out
	cmd.Stderr = out
	return cmd
}

// ensurePrefix cria o prefixo uma vez. O marcador só é gravado no fim, para uma
// primeira execução interrompida ser refeita.
func (l *launcher) ensurePrefix() error {
	marker := filepath.Join(l.prefix, ".wyd-pronto")
	if _, err := os.Stat(marker); err == nil {
		return nil
	}
	if err := os.MkdirAll(l.prefix, 0o755); err != nil {
		return fmt.Errorf("não foi possível criar %s: %w", l.prefix, err)
	}
	stop := showProgress("Preparando o jogo para a primeira execução. Isso leva cerca de um minuto.")
	defer stop()

	logPath := filepath.Join(l.dataDir, "preparo.log")
	log, err := os.Create(logPath)
	if err != nil {
		return fmt.Errorf("não foi possível criar %s: %w", logPath, err)
	}
	defer log.Close()

	steps := [][]string{
		{"wineboot", "-i"},
		{"reg", "add", `HKCU\Software\Wine\WineDbg`, "/v", "ShowCrashDialog", "/t", "REG_DWORD", "/d", "0", "/f"},
	}
	for _, step := range steps {
		if err := runWithTimeout(l.wine(log, step...), bootTimeout); err != nil {
			return fmt.Errorf("falha ao preparar o Wine (%s): %v\nDetalhes em %s", step[0], err, logPath)
		}
	}
	server := exec.Command(filepath.Join(l.wineDir, "wineserver"), "-w")
	server.Env = l.env()
	if err := runWithTimeout(server, bootTimeout); err != nil {
		return fmt.Errorf("falha ao finalizar o Wine: %v\nDetalhes em %s", err, logPath)
	}
	return os.WriteFile(marker, nil, 0o644)
}

func (l *launcher) browserWine(out io.Writer, args ...string) *exec.Cmd {
	cmd := l.wine(out, args...)
	cmd.Env = append(cmd.Env, "WINEDLLOVERRIDES="+browserOverrides)
	return cmd
}

// ensureGecko instala o motor da página do launcher, também em prefixos criados
// antes dele. Uma falha não impede o jogo: o launcher mostra a arte local.
func (l *launcher) ensureGecko() {
	marker := filepath.Join(l.prefix, geckoMarker)
	if _, err := os.Stat(marker); err == nil {
		return
	}
	installer := filepath.Join(l.gameDir, filepath.FromSlash(strings.ReplaceAll(geckoInstaller, `\`, "/")))
	if _, err := os.Stat(installer); err != nil {
		return
	}
	stop := showProgress("Preparando a página do launcher. Isso leva alguns segundos.")
	defer stop()

	log, err := os.Create(filepath.Join(l.dataDir, "gecko.log"))
	if err != nil {
		return
	}
	defer log.Close()
	// O prefixo nasce com o mshtml desligado, e sem o registro dele o Wine não
	// associa text/html a nada: a página baixa, mas a área fica branca.
	steps := [][]string{
		{"msiexec", "/i", geckoInstaller, "/qn"},
		{"regsvr32", "/s", "mshtml.dll"},
	}
	for _, step := range steps {
		if err := runWithTimeout(l.browserWine(log, step...), bootTimeout); err != nil {
			fmt.Fprintln(log, step[0]+":", err)
			return
		}
	}
	os.WriteFile(marker, nil, 0o644)
}

func runWithTimeout(cmd *exec.Cmd, limit time.Duration) error {
	if err := cmd.Start(); err != nil {
		return err
	}
	timer := time.AfterFunc(limit, func() { cmd.Process.Kill() })
	defer timer.Stop()
	return cmd.Wait()
}

// configure abre o launcher e diz se o jogador pediu para jogar. O jogo
// é aberto depois, por play, para a saída dele continuar indo para o log.
func (l *launcher) configure() (bool, error) {
	logPath := filepath.Join(l.dataDir, "configuracoes.log")
	log, err := os.Create(logPath)
	if err != nil {
		return false, fmt.Errorf("não foi possível criar %s: %w", logPath, err)
	}
	defer log.Close()

	err = l.browserWine(log, settingsProgram, "--configurar").Run()
	var exit *exec.ExitError
	switch {
	case err == nil:
		return true, nil
	case errors.As(err, &exit) && exit.ExitCode() == settingsClosed:
		return false, nil
	}
	return false, fmt.Errorf("não foi possível abrir o launcher: %v\nDetalhes em %s", err, logPath)
}

func (l *launcher) play(args []string) int {
	logPath := filepath.Join(l.dataDir, "wyd.log")
	os.Rename(logPath, logPath+".anterior")
	log, err := os.Create(logPath)
	if err != nil {
		showError(fmt.Sprintf("não foi possível criar %s: %v", logPath, err))
		return 1
	}
	defer log.Close()

	var out io.Writer = log
	if info, err := os.Stderr.Stat(); err == nil && info.Mode()&os.ModeCharDevice != 0 {
		out = io.MultiWriter(log, os.Stderr)
	}
	if err := l.wine(out, append([]string{"WYD.exe"}, args...)...).Run(); err != nil && !isExitError(err) {
		showError(fmt.Sprintf("não foi possível abrir o jogo: %v", err))
		return 1
	}
	if logHasCrash(logPath) {
		showError("O jogo fechou por causa de um erro.\nAo reportar, envie o arquivo:\n" + logPath)
		return 1
	}
	return 0
}

func logHasCrash(path string) bool {
	data, err := os.ReadFile(path)
	return err == nil && strings.Contains(string(data), crashMarker)
}

// O código de saída do jogo não indica falha: quedas são detectadas pelo log.
func isExitError(err error) bool {
	var exit *exec.ExitError
	return errors.As(err, &exit)
}
