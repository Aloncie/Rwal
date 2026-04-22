# setup-windows.ps1 - Rwal Windows Dependency Installer
# Run: powershell -ExecutionPolicy Bypass -File setup-windows.ps1

param(
    [switch]$SkipWinget,
    [switch]$SkipVcpkg,
    [switch]$Help
)

if ($Help) {
    Write-Host @"
Rwal Windows Setup Script
=========================
Installs or verifies all dependencies needed to build Rwal on Windows.

Options:
  -SkipWinget    Skip winget installation (use manual URLs instead)
  -SkipVcpkg     Skip vcpkg setup (you'll need to provide libraries manually)
  -Help          Show this help message

What this script does:
  1. Checks for Git, CMake, C++ compiler
  2. Installs missing tools via winget (or shows manual URLs)
  3. Bootstraps vcpkg for C++ libraries
  4. Installs nlohmann-json and curl via vcpkg
  5. Verifies Qt6 installation (shows installer URL if missing)
"@
    exit 0
}

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Colors for output
function Write-Info { Write-Host "[INFO] " -ForegroundColor Cyan -NoNewline; Write-Host $args }
function Write-Success { Write-Host "[OK]   " -ForegroundColor Green -NoNewline; Write-Host $args }
function Write-Warning { Write-Host "[WARN] " -ForegroundColor Yellow -NoNewline; Write-Host $args }
function Write-ErrorMsg { Write-Host "[ERROR]" -ForegroundColor Red -NoNewline; Write-Host " $args" }

Write-Host @"
========================================
   Rwal Windows Dependency Installer
========================================
"@ -ForegroundColor Magenta

# ============================================================
# 1. Check PowerShell version (require 5.1+)
# ============================================================
if ($PSVersionTable.PSVersion.Major -lt 5) {
    Write-ErrorMsg "PowerShell 5.1 or higher required. You have $($PSVersionTable.PSVersion)."
    exit 1
}

# ============================================================
# 2. Check for Git
# ============================================================
Write-Info "Checking for Git..."
$gitPath = Get-Command git -ErrorAction SilentlyContinue
if ($gitPath) {
    $gitVersion = (git --version) -replace "git version ", ""
    Write-Success "Git $gitVersion found at $($gitPath.Source)"
} else {
    Write-Warning "Git not found."
    if (-not $SkipWinget -and (Get-Command winget -ErrorAction SilentlyContinue)) {
        Write-Info "Installing Git via winget..."
        winget install --silent --exact Git.Git
        Write-Success "Git installed. Please restart your terminal after setup."
    } else {
        Write-Warning "Please install Git manually: https://git-scm.com/download/win"
    }
}

# ============================================================
# 3. Check for CMake
# ============================================================
Write-Info "Checking for CMake..."
$cmakePath = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakePath) {
    $cmakeVersion = (cmake --version | Select-Object -First 1) -replace "cmake version ", ""
    Write-Success "CMake $cmakeVersion found at $($cmakePath.Source)"
} else {
    Write-Warning "CMake not found."
    if (-not $SkipWinget -and (Get-Command winget -ErrorAction SilentlyContinue)) {
        Write-Info "Installing CMake via winget..."
        winget install --silent --exact Kitware.CMake
        Write-Success "CMake installed. Please restart your terminal after setup."
    } else {
        Write-Warning "Please install CMake manually: https://cmake.org/download/"
    }
}

# ============================================================
# 4. Check for C++ Compiler (Visual Studio or MinGW)
# ============================================================
Write-Info "Checking for C++ compiler..."
$hasCompiler = $false

# Check for Visual Studio
$vsPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath 2>$null
if ($vsPath) {
    Write-Success "Visual Studio found at $vsPath"
    $hasCompiler = $true
}

# Check for MinGW
$gppPath = Get-Command g++ -ErrorAction SilentlyContinue
if ($gppPath) {
    $gppVersion = (g++ --version | Select-Object -First 1)
    Write-Success "MinGW g++ found: $gppVersion"
    $hasCompiler = $true
}

# Check for Clang
$clangPath = Get-Command clang++ -ErrorAction SilentlyContinue
if ($clangPath) {
    Write-Success "Clang found at $($clangPath.Source)"
    $hasCompiler = $true
}

if (-not $hasCompiler) {
    Write-Warning "No C++ compiler found."
    Write-Host "  Options:"
    Write-Host "    1. Install Visual Studio Build Tools: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022"
    Write-Host "    2. Install MinGW-w64: https://www.mingw-w64.org/"
    Write-Host "    3. Install via winget: 'winget install Microsoft.VisualStudio.2022.BuildTools'"
}

# ============================================================
# 5. Setup vcpkg for C++ libraries
# ============================================================
$vcpkgRoot = "$ScriptDir\vcpkg"
if (-not $SkipVcpkg) {
    Write-Info "Setting up vcpkg..."
    
    if (-not (Test-Path $vcpkgRoot)) {
        Write-Info "Cloning vcpkg..."
        git clone https://github.com/microsoft/vcpkg.git $vcpkgRoot
    }
    
    Push-Location $vcpkgRoot
    try {
        Write-Info "Bootstrapping vcpkg..."
        .\bootstrap-vcpkg.bat -disableMetrics
        Write-Success "vcpkg bootstrapped."
        
        Write-Info "Installing nlohmann-json..."
        .\vcpkg.exe install nlohmann-json:x64-windows
        
        Write-Info "Installing curl..."
        .\vcpkg.exe install curl:x64-windows
        
        Write-Success "vcpkg libraries installed."
        Write-Info "To use vcpkg with CMake, add: -DCMAKE_TOOLCHAIN_FILE=$vcpkgRoot\scripts\buildsystems\vcpkg.cmake"
    } finally {
        Pop-Location
    }
} else {
    Write-Warning "Skipping vcpkg. You'll need to provide nlohmann-json and curl manually."
}

# ============================================================
# 6. Check for Qt6
# ============================================================
Write-Info "Checking for Qt6..."
$qtEnv = [Environment]::GetEnvironmentVariable("QTDIR", "User")
if (-not $qtEnv) {
    $qtEnv = [Environment]::GetEnvironmentVariable("QTDIR", "Machine")
}

if ($qtEnv -and (Test-Path $qtEnv)) {
    Write-Success "Qt6 found at $qtEnv"
} else {
    Write-Warning "Qt6 not found in QTDIR environment variable."
    Write-Host "  Please install Qt6:"
    Write-Host "    - Qt Online Installer: https://www.qt.io/download-open-source"
    Write-Host "    - Choose Qt 6 with MinGW or MSVC component"
    Write-Host "  After installation, set QTDIR environment variable to Qt directory"
    Write-Host "  (e.g., C:\Qt\6.11\mingw81_64)"
}

# ============================================================
# 7. Summary
# ============================================================
Write-Host @"
========================================
   Setup Complete
========================================
"@ -ForegroundColor Magenta

Write-Host "Next steps:"
Write-Host "  1. Restart your terminal"
Write-Host "  2. Build Rwal:"
Write-Host "     mkdir build && cd build"
Write-Host "     cmake .. -DCMAKE_TOOLCHAIN_FILE=$vcpkgRoot\scripts\buildsystems\vcpkg.cmake"
Write-Host "     cmake --build . --config Release"
Write-Host ""
Write-Host "Note: TUI is disabled on Windows, only CLI is supported."

if (-not $hasCompiler -or -not $qtEnv) {
    Write-Warning "Some dependencies still need manual installation (see warnings above)."
}

