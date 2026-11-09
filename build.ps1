# Compila o cliente para Windows x64 com o Visual Studio 2022 e, opcionalmente,
# instala na pasta do jogo. O equivalente no Linux e o wyd-linux.sh.
#
#   powershell -ExecutionPolicy Bypass -File build.ps1                # Release
#   powershell -ExecutionPolicy Bypass -File build.ps1 -Debug
#   powershell -ExecutionPolicy Bypass -File build.ps1 -Deploy        # e copia para o jogo
#   powershell -ExecutionPolicy Bypass -File build.ps1 -Deploy -ReleaseDir 'D:\WYD'
#
# Tudo vai para Build\x64\<Config>\: WYD.exe, JogarWYD.exe (o launcher, que precisa
# do Go) e as DLLs do D3DX 43. -Deploy copia esses arquivos para a pasta do jogo,
# ..\wyd-others\Release Cliente por padrao. Os jogadores abrem o JogarWYD.exe.
param(
    [switch]$Debug,
    [switch]$Deploy,
    [string]$ReleaseDir = (Join-Path $PSScriptRoot '..\wyd-others\Release Cliente')
)

$ErrorActionPreference = 'Stop'
$config = if ($Debug) { 'Debug' } else { 'Release' }

if ($Deploy -and -not (Test-Path -LiteralPath $ReleaseDir -PathType Container)) {
    throw "Pasta do jogo nao encontrada: $ReleaseDir"
}

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path -LiteralPath $vswhere)) { throw "Visual Studio Build Tools nao encontrado (falta vswhere.exe)." }
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw "Build Tools sem o compilador C++ (VC.Tools.x86.x64)." }
$msbuild = Join-Path $vs 'MSBuild\Current\Bin\MSBuild.exe'

$root = $PSScriptRoot
$out = Join-Path $root "Build\x64\$config\"
New-Item -ItemType Directory -Force $out | Out-Null
$files = @('WYD.exe', 'D3DX9_43.dll', 'D3DCompiler_43.dll')

# O launcher vem primeiro: leva segundos, e uma falha aqui evita esperar o C++.
if (Get-Command go -ErrorAction SilentlyContinue) {
    Push-Location (Join-Path $root 'Projects\WYDLauncher')
    try {
        $env:CGO_ENABLED = '0'
        & go test .
        if ($LASTEXITCODE -ne 0) { throw "testes do JogarWYD falharam ($LASTEXITCODE)" }
        & go build -trimpath -ldflags '-s -w -H=windowsgui' -o (Join-Path $out 'JogarWYD.exe') .
        if ($LASTEXITCODE -ne 0) { throw "build do JogarWYD falhou ($LASTEXITCODE)" }
    } finally { Pop-Location }
    $files += 'JogarWYD.exe'
} else {
    Write-Warning "Go nao encontrado: JogarWYD.exe nao foi gerado."
}

& $msbuild (Join-Path $root 'TM.sln') /m /nologo /verbosity:minimal `
    "/p:Configuration=$config" /p:Platform=x64 "/p:OutDir=$out"
if ($LASTEXITCODE -ne 0) { throw "build falhou ($LASTEXITCODE)" }
Write-Host "OK: $out"

if ($Deploy) {
    foreach ($name in $files) {
        Copy-Item -LiteralPath (Join-Path $out $name) -Destination $ReleaseDir -Force
    }
    foreach ($name in @('serverlist.bin', 'Launcher.bmp')) {
        if (-not (Test-Path -LiteralPath (Join-Path $ReleaseDir $name))) {
            Write-Warning "$name nao esta em $ReleaseDir"
        }
    }
    Write-Host "Instalado em: $ReleaseDir"
}
