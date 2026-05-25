# build.ps1 – Rwal build wrapper for Windows
# Reloads saved environment variables and runs cmake --preset

$ErrorActionPreference = "Stop"

# 1. Reload VCPKG_ROOT and QTDIR from the registry (User scope)
$vcpkgRoot = [Environment]::GetEnvironmentVariable("VCPKG_ROOT", "User")
$qtDir = [Environment]::GetEnvironmentVariable("QTDIR", "User")

# 2. If they are missing, the user must run setup first
if (-not $vcpkgRoot) {
    Write-Host "[ERROR] VCPKG_ROOT not set. Please run: python setup.py" -ForegroundColor Red
    exit 1
}
if (-not $qtDir) {
    Write-Host "[ERROR] QTDIR not set. Please run: python setup.py" -ForegroundColor Red
    exit 1
}

# Push into the current session
$env:VCPKG_ROOT = $vcpkgRoot
$env:QTDIR = $qtDir

Write-Host "[INFO] VCPKG_ROOT = $vcpkgRoot" -ForegroundColor Cyan
Write-Host "[INFO] QTDIR = $qtDir" -ForegroundColor Cyan

# 3. Build
cmake --preset windows-release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build build/windows-release --preset windows-release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[SUCCESS] Build complete." -ForegroundColor Green
