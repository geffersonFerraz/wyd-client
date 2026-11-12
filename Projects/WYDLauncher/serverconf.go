package main

import (
	"errors"
	"fmt"
	"io/fs"
	"net"
	"net/netip"
	"net/url"
	"os"
	"regexp"
	"strconv"
	"strings"
)

// Servidor.txt (IP ou nome DNS) guarda o endereço do servidor escolhido nas Configurações do
// launcher: uma linha "ip" ou "ip:porta" (a porta é a do site, 80 se omitida).
// Quando existe, ele vale mais que o serverlist.bin: o launcher baixa os
// arquivos dele e reescreve o serverlist.bin com esse IP, que é o que o jogo lê.
// Sem o arquivo, o endereço continua vindo do serverlist.bin, como antes.
const (
	serverAddressFile = "Servidor.txt"
	maxAddressLength  = 21 // 255.255.255.255:65535
	serverListEntries = 11 // MAX_SERVERNUMBER: [0] é a URL do servNN.htm, o resto são canais.
)

var hostLabel = regexp.MustCompile(`^[A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?$`)

// validHost aceita IPv4 ou nome DNS (rótulos de letras, números e hífen).
func validHost(host string) bool {
	if ip, err := netip.ParseAddr(host); err == nil {
		return ip.Is4()
	}
	if host == "" || len(host) > 253 || strings.Trim(host, "0123456789.") == "" {
		return false // Só números e pontos tem de ser um IPv4 válido, como 300.1.1.1 não é.
	}
	for _, label := range strings.Split(host, ".") {
		if !hostLabel.MatchString(label) {
			return false
		}
	}
	return true
}

// normalizeAddress aceita "host" ou "host:porta", com ou sem "http://"; host é
// um IPv4 ou um nome DNS (o cliente resolve nomes com o DNS do computador).
func normalizeAddress(input string) (string, error) {
	s := strings.TrimSpace(input)
	s = strings.TrimPrefix(s, "http://")
	s = strings.TrimRight(s, "/")
	host, port := s, ""
	if h, p, err := net.SplitHostPort(s); err == nil {
		host, port = h, p
	}
	if !validHost(host) {
		return "", errors.New("Informe o IP ou o nome do servidor, por exemplo 192.168.0.10, wyd.exemplo.com ou 192.168.0.10:80.")
	}
	if ip, err := netip.ParseAddr(host); err == nil {
		host = ip.String()
	} else {
		host = strings.ToLower(host)
	}
	if port == "" {
		return host, nil
	}
	n, err := strconv.Atoi(port)
	if err != nil || n < 1 || n > 65535 {
		return "", errors.New("Porta inválida. Use um número de 1 a 65535.")
	}
	return host + ":" + strconv.Itoa(n), nil
}

// loadServerAddress devolve "" quando o jogador não definiu um endereço (ou o
// arquivo está inválido: o serverlist.bin volta a valer).
func loadServerAddress(dir string) string {
	data, err := os.ReadFile(localPath(dir, serverAddressFile))
	if err != nil {
		return ""
	}
	line, _, _ := strings.Cut(string(data), "\n")
	address, err := normalizeAddress(line)
	if err != nil {
		return ""
	}
	return address
}

// saveServerAddress grava o endereço; vazio apaga o arquivo e devolve o
// controle ao serverlist.bin.
func saveServerAddress(dir, input string) error {
	path := localPath(dir, serverAddressFile)
	if strings.TrimSpace(input) == "" {
		if err := os.Remove(path); err != nil && !errors.Is(err, fs.ErrNotExist) {
			return fmt.Errorf("não foi possível apagar %s: %w", path, err)
		}
		return nil
	}
	address, err := normalizeAddress(input)
	if err != nil {
		return err
	}
	return replaceFile(path, []byte(address+"\n"))
}

// currentServerAddress é o endereço em uso: o das Configurações ou, sem ele, o
// do serverlist.bin local. Vazio se nenhum dos dois existe.
func currentServerAddress(dir string) string {
	if address := loadServerAddress(dir); address != "" {
		return address
	}
	data, err := os.ReadFile(localPath(dir, serverListFile))
	if err != nil {
		return ""
	}
	base, err := serverBase(data)
	if err != nil {
		return ""
	}
	return strings.TrimPrefix(strings.TrimPrefix(base, "http://"), "https://")
}

// launcherBase devolve "http://host[:porta]" do site do servidor.
func launcherBase(dir string) (string, error) {
	if address := loadServerAddress(dir); address != "" {
		return "http://" + address, nil
	}
	data, err := os.ReadFile(localPath(dir, serverListFile))
	if err != nil {
		return "", err
	}
	return serverBase(data)
}

func decodeListEntry(entry []byte) string {
	plain := make([]byte, len(entry))
	for i, b := range entry {
		plain[i] = b - serverListKey[len(serverListKey)-1-i]
	}
	text, _, _ := strings.Cut(string(plain), "\x00")
	for _, c := range []byte(text) {
		if c < 0x20 || c > 0x7e {
			return ""
		}
	}
	return text
}

func encodeListEntry(entry []byte, text string) {
	for i := range entry {
		var c byte
		if i < len(text) {
			c = text[i]
		}
		entry[i] = c + serverListKey[len(serverListKey)-1-i]
	}
}

// rewriteServerList troca o IP de todas as entradas do serverlist.bin pelo do
// launcher: a URL do servNN.htm vira http://<address>/..., e cada canal
// ("ip" ou "ip:porta") fica com o IP novo e a porta que já tinha. Entradas
// vazias ou que não sejam endereço (IP ou nome) ficam como estão.
func rewriteServerList(data []byte, address string) []byte {
	out := append([]byte(nil), data...)
	if len(out) != serverListSize {
		return out
	}
	host := address
	if h, _, err := net.SplitHostPort(address); err == nil {
		host = h
	}
	for i := 0; i < serverListSize/len(serverListKey); i++ {
		entry := out[i*len(serverListKey) : (i+1)*len(serverListKey)]
		text := decodeListEntry(entry)
		if text == "" {
			continue
		}
		var next string
		if i%serverListEntries == 0 {
			u, err := url.Parse(text)
			if err != nil || (u.Scheme != "http" && u.Scheme != "https") || u.Host == "" {
				continue
			}
			u.Host = address
			next = u.String()
		} else {
			oldHost, port := text, ""
			if h, p, err := net.SplitHostPort(text); err == nil {
				oldHost, port = h, p
			}
			if !validHost(oldHost) {
				continue
			}
			next = host
			if port != "" {
				next = net.JoinHostPort(host, port)
			}
		}
		if len(next) < len(serverListKey) {
			encodeListEntry(entry, next)
		}
	}
	return out
}
