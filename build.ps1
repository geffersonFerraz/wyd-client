# Compila o cliente (TM.sln) com o Visual Studio 2022 Build Tools.
#
#   powershell -ExecutionPolicy Bypass -File build.ps1            # Release
#   powershell -ExecutionPolicy Bypass -File build.ps1 -Debug
#
# O cliente e somente x64. O WYD.exe e as DLLs do D3DX 43 vao para
# Build\x64\<Config>\. Para testar no jogo, copie WYD.exe, D3DX9_43.dll e
# D3DCompiler_43.dll para Release Cliente\.
param([switch]$Debug)

$ErrorActionPreference = 'Stop'
$config = if ($Debug) { 'Debug' } else { 'Release' }

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw "Visual Studio Build Tools nao encontrado (falta vswhere.exe)." }
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw "Build Tools sem o compilador C++ (VC.Tools.x86.x64)." }
$msbuild = Join-Path $vs 'MSBuild\Current\Bin\MSBuild.exe'

$root = $PSScriptRoot
$out = Join-Path $root "Build\x64\$config\"
New-Item -ItemType Directory -Force $out | Out-Null

& $msbuild (Join-Path $root 'TM.sln') /m /nologo /verbosity:minimal `
    "/p:Configuration=$config" /p:Platform=x64 "/p:OutDir=$out"
if ($LASTEXITCODE -ne 0) { throw "build falhou ($LASTEXITCODE)" }
Write-Host "OK: $out`WYD.exe"
