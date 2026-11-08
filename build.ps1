# Compila o cliente (TM.sln) com o Visual Studio 2022 Build Tools.
#
#   powershell -ExecutionPolicy Bypass -File build.ps1            # Release
#   powershell -ExecutionPolicy Bypass -File build.ps1 -Debug
#
# O WYD.exe vai para Source Cliente\Build\<Config>\ (o projeto, sozinho,
# gravaria em Release Cliente\ no Debug e numa pasta de outro PC no Release).
# Copie o exe para Release Cliente\ quando quiser testar no jogo.
param([switch]$Debug)

$ErrorActionPreference = 'Stop'
$config = if ($Debug) { 'Debug' } else { 'Release' }

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw "Visual Studio Build Tools nao encontrado (falta vswhere.exe)." }
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw "Build Tools sem o compilador C++ (VC.Tools.x86.x64)." }
$msbuild = Join-Path $vs 'MSBuild\Current\Bin\MSBuild.exe'

$root = $PSScriptRoot
$out = Join-Path $root "Build\$config\"
New-Item -ItemType Directory -Force $out | Out-Null

& $msbuild (Join-Path $root 'TM.sln') /m /nologo /verbosity:minimal `
    "/p:Configuration=$config" /p:Platform=x86 "/p:OutDir=$out" # x86 na solution = Win32 no projeto
if ($LASTEXITCODE -ne 0) { throw "build falhou ($LASTEXITCODE)" }
Write-Host "OK: $out`WYD.exe"
