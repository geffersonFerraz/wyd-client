#!/usr/bin/env bash
# Compila e executa o cliente no Linux (Ubuntu 24.04 ou mais novo).
#
#   ./wyd-linux.sh deps    # instala as dependências (usa sudo)
#   ./wyd-linux.sh build   # gera o WYD.exe e copia para a pasta de release
#   ./wyd-linux.sh bundle  # põe o Wine e os lançadores na pasta de release
#   ./wyd-linux.sh run     # abre o jogo com Wine
#   ./wyd-linux.sh debug   # abre o jogo e grava o backtrace de crash em build-linux/wyd-debug.log
#
# Variáveis: WYD_RELEASE_DIR (pasta do jogo), WINEPREFIX, WINE.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT/build-linux"
RELEASE_DIR="${WYD_RELEASE_DIR:-$ROOT/../wyd-others/Release Cliente}"

export WINEPREFIX="${WINEPREFIX:-$HOME/.local/share/wyd-linux/prefix}"

WINE_VERSION="10.0"
WINE_ARCHIVE="wine-$WINE_VERSION-amd64-wow64.tar.xz"
WINE_SHA256="aeebbbf239e548f0136f1cd72a2e109dd9a572a8f703da3c09fa40d74d5c255f"
WINE_URL="https://github.com/Kron4ek/Wine-Builds/releases/download/$WINE_VERSION/$WINE_ARCHIVE"
WINE_SOURCE_URL="https://dl.winehq.org/wine/source/10.0/wine-$WINE_VERSION.tar.xz"
WINE_LICENSE_URL="https://raw.githubusercontent.com/wine-mirror/wine/wine-$WINE_VERSION"
# Motor HTML da página do launcher. A versão e o hash são os que o Wine $WINE_VERSION exige.
GECKO_VERSION="2.47.4"
GECKO_ARCHIVE="wine-gecko-$GECKO_VERSION-x86_64.msi"
GECKO_SHA256="e590b7d988a32d6aa4cf1d8aa3aa3d33766fdd4cf4c89c2dcc2095ecb28d066f"
GECKO_URL="https://dl.winehq.org/wine/wine-gecko/$GECKO_VERSION/$GECKO_ARCHIVE"

require() {
    command -v "$1" >/dev/null 2>&1 && return
    echo "Comando '$1' não encontrado. Rode antes: $0 deps" >&2
    exit 1
}

# O pacote do Ubuntu 26.04 instala wine64-stable, sem o comando "wine".
find_wine() {
    local candidate
    for candidate in ${WINE:-} wine wine64-stable; do
        if command -v "$candidate" >/dev/null 2>&1; then
            WINE="$candidate"
            return
        fi
    done
    echo "Wine não encontrado. Rode antes: $0 deps" >&2
    exit 1
}

cmd_deps() {
    sudo apt-get update
    sudo apt-get install -y --no-install-recommends \
        cmake make mingw-w64 wine wine64 gstreamer1.0-plugins-good golang-go curl xz-utils
}

cmd_build() {
    require cmake
    require x86_64-w64-mingw32-g++-posix
    [[ -d "$RELEASE_DIR" ]] || { echo "Pasta de release não encontrada: $RELEASE_DIR" >&2; exit 1; }
    cmake -S "$ROOT" -B "$BUILD_DIR" -DWYD_RELEASE_DIR="$RELEASE_DIR"
    cmake --build "$BUILD_DIR" --target deploy --parallel "$(nproc)"
    echo "OK: $RELEASE_DIR/WYD.exe"
}

# O Wine embutido fica em wine/, com a licença e o endereço do código-fonte (LGPL).
install_wine() {
    local target="$RELEASE_DIR/wine" archive="$BUILD_DIR/$WINE_ARCHIVE" unpack="$BUILD_DIR/wine-unpack"
    [[ -x "$target/bin/wine" ]] && return
    mkdir -p "$BUILD_DIR"
    if ! echo "$WINE_SHA256  $archive" | sha256sum -c --status 2>/dev/null; then
        curl -fL --retry 3 -o "$archive" "$WINE_URL"
        echo "$WINE_SHA256  $archive" | sha256sum -c --status || { echo "Checksum inválido: $archive" >&2; exit 1; }
    fi
    rm -rf "$unpack"
    mkdir -p "$unpack"
    tar -xf "$archive" -C "$unpack" --strip-components=1
    curl -fsSL -o "$unpack/LICENSE" "$WINE_LICENSE_URL/LICENSE"
    curl -fsSL -o "$unpack/COPYING.LIB" "$WINE_LICENSE_URL/COPYING.LIB"
    cat > "$unpack/CODIGO-FONTE.txt" <<EOF
Wine $WINE_VERSION, distribuído sob a LGPL (veja LICENSE e COPYING.LIB).
Código-fonte: $WINE_SOURCE_URL
Build usado: $WINE_URL
Scripts do build: https://github.com/Kron4ek/Wine-Builds
EOF
    mv "$unpack" "$target"
}

# O WYD-Linux instala este pacote no prefixo. Licença MPL, fontes no endereço abaixo.
install_gecko() {
    local target="$RELEASE_DIR/wine/share/wine/gecko" archive="$BUILD_DIR/$GECKO_ARCHIVE"
    echo "$GECKO_SHA256  $target/$GECKO_ARCHIVE" | sha256sum -c --status >/dev/null 2>&1 && return
    mkdir -p "$BUILD_DIR" "$target"
    if ! echo "$GECKO_SHA256  $archive" | sha256sum -c --status 2>/dev/null; then
        curl -fL --retry 3 -o "$archive" "$GECKO_URL"
        echo "$GECKO_SHA256  $archive" | sha256sum -c --status || { echo "Checksum inválido: $archive" >&2; exit 1; }
    fi
    cp "$archive" "$target/$GECKO_ARCHIVE"
    cat > "$target/SOURCE.txt" <<EOF
Wine Gecko $GECKO_VERSION, distribuído sob a MPL.
Pacote e código-fonte: https://dl.winehq.org/wine/wine-gecko/$GECKO_VERSION/
EOF
}

cmd_bundle() {
    require go
    require curl
    [[ -f "$RELEASE_DIR/WYD.exe" ]] || { echo "Rode antes: $0 build" >&2; exit 1; }
    install_wine
    install_gecko
    (
        cd "$ROOT/Projects/WYDLauncher"
        CGO_ENABLED=0 go build -trimpath -ldflags '-s -w' -o "$RELEASE_DIR/WYD-Linux" .
        CGO_ENABLED=0 GOOS=windows GOARCH=amd64 go build -trimpath -ldflags '-s -w -H=windowsgui' -o "$RELEASE_DIR/JogarWYD.exe" .
    )
    # O Wine não precisa do bit de execução. Sem ele, o Linux não tenta rodar o .exe
    # como script de shell, o que criaria arquivos com nomes quebrados na pasta.
    chmod -x "$RELEASE_DIR/JogarWYD.exe"
    echo "OK: $RELEASE_DIR/WYD-Linux e JogarWYD.exe"
}

cmd_run() {
    find_wine
    [[ -f "$RELEASE_DIR/WYD.exe" ]] || { echo "Rode antes: $0 build" >&2; exit 1; }
    if [[ ! -f "$WINEPREFIX/system.reg" ]]; then
        mkdir -p "$(dirname "$WINEPREFIX")"
        WINEDLLOVERRIDES="mscoree,mshtml=" "$WINE" wineboot -i
    fi
    cd "$RELEASE_DIR"
    WINEDLLOVERRIDES="d3dx9_43=n,b;d3dcompiler_43=n,b;mscoree,mshtml=" exec "$WINE" WYD.exe "$@"
}

# Desliga a janela de erro do Wine; as linhas [wyd-crash] de CrashLog.cpp vão para o log.
cmd_debug() {
    find_wine
    "$WINE" reg add 'HKCU\Software\Wine\WineDbg' /v ShowCrashDialog /t REG_DWORD /d 0 /f >/dev/null 2>&1
    mkdir -p "$BUILD_DIR"
    cmd_run "$@" 2>&1 | tee "$BUILD_DIR/wyd-debug.log"
}

case "${1:-}" in
    deps)  cmd_deps ;;
    build) cmd_build ;;
    bundle) cmd_bundle ;;
    run)   shift; cmd_run "$@" ;;
    debug) shift; cmd_debug "$@" ;;
    *)     sed -n '2,10p' "$0" | sed 's/^# \?//'; exit 2 ;;
esac
