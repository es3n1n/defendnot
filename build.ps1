<#
    defendnot - convenience build script
    Works with any Visual Studio that has the "Desktop development with C++" workload
    (VS 2022 = v143, VS 2026 = v145). The project files use $(DefaultPlatformToolset),
    so the correct toolset is picked automatically.

    Usage:
        .\build.ps1                  # Release x64 (default)
        .\build.ps1 -Platform ARM64
        .\build.ps1 -Configuration Debug
        .\build.ps1 -Platform x86    # mapped to Win32
#>
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',
    [ValidateSet('x64', 'x86', 'ARM64')]
    [string]$Platform = 'x64'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path

# Locate vswhere / MSBuild
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe not found. Please install Visual Studio."
}
$msbuild = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' |
    Select-Object -First 1
if (-not $msbuild) {
    throw "MSBuild.exe not found. Please install the 'Desktop development with C++' workload."
}

# Ensure argparse submodule is present
$argparse = Join-Path $root 'vendor\argparse\include\argparse\argparse.hpp'
if (-not (Test-Path $argparse)) {
    Write-Warning "vendor\argparse missing, trying to init submodules..."
    Push-Location $root
    try {
        git -c http.sslBackend=openssl submodule update --init --recursive
    } finally {
        Pop-Location
    }
    if (-not (Test-Path $argparse)) {
        throw "argparse still missing. Run: git submodule update --init --recursive"
    }
}

$slnPlatform = if ($Platform -eq 'x86') { 'Win32' } else { $Platform }

Write-Host "==> MSBuild: $msbuild"
Write-Host "==> Configuration: $Configuration | Platform: $slnPlatform"

& $msbuild (Join-Path $root 'defendnot.sln') `
    "/p:Configuration=$Configuration" `
    "/p:Platform=$slnPlatform" `
    -t:rebuild -m:1 -verbosity:minimal -nologo

if ($LASTEXITCODE -ne 0) {
    throw "Build failed (exit $LASTEXITCODE)"
}

Write-Host ""
Write-Host "Build finished. Output: $(Join-Path $root "out\$slnPlatform")"
Get-ChildItem (Join-Path $root "out\$slnPlatform") -File |
    Where-Object { $_.Extension -in '.dll', '.exe' } |
    Select-Object Name, Length |
    Format-Table -AutoSize
