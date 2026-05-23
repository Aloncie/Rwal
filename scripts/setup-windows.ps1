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
  1. Checks for Git, CMake, Qt6, vcpkg and C++ compiler.
  2. Installs missing tools via winget (or shows manual URLs)
  3. Bootstraps vcpkg for C++ libraries
  4. Installs nlohmann-json and curl via vcpkg
  5. Verifies Qt6 installation (shows installer URL if missing)
"@
    exit 0
}

# Ensure any unhandled pipeline error safely terminates script execution
$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# ============================================================
# Log Formatting Functions
# ============================================================
function Write-Info    { Write-Host "[INFO] " -ForegroundColor Cyan -NoNewline; Write-Host "$args" }
function Write-Success { Write-Host "[OK]   " -ForegroundColor Green -NoNewline; Write-Host "$args" }
function Write-Warning { Write-Host "[WARN] " -ForegroundColor Yellow -NoNewline; Write-Host "$args" }
function Write-ErrorMsg { Write-Host "[ERROR] " -ForegroundColor Red -NoNewline; Write-Host "$args" }

Write-Host @"
========================================
   Rwal Windows Dependency Installer
========================================
"@ -ForegroundColor Magenta

# ============================================================
# 1. Check PowerShell version & Administrative privileges
# ============================================================

# Verify PowerShell engine version matches modern scripting capabilities (5.1+)
if ($PSVersionTable.PSVersion.Major -lt 5) {
    Write-ErrorMsg "PowerShell 5.1 or higher required. You have $($PSVersionTable.PSVersion)."
    exit 1
}

# Verify that the script is running with elevated permissions for WinGet success
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Warning "This script is not running as Administrator."
    Write-Warning "Some automated installations via WinGet may fail or require UAC prompts."
    Write-Host "Recommendation: Restart your terminal as Administrator before continuing.`n" -ForegroundColor Gray
} else {
    Write-Success "Running with elevated Administrator privileges."
}

# ============================================================
# Helper function to reload PATH environment variables dynamically
# ============================================================
function Refresh-SessionPath {
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
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
        Start-Process winget -ArgumentList "install --silent --exact Git.Git" -Wait -NoNewWindow
        Refresh-SessionPath
        
        # Verify if install succeeded immediately
        if (Get-Command git -ErrorAction SilentlyContinue) {
            Write-Success "Git successfully installed and loaded into this session."
        } else {
            Write-Warning "Git installed via winget, but you must restart your terminal to use it."
        }
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
        Start-Process winget -ArgumentList "install --silent --exact Kitware.CMake" -Wait -NoNewWindow
        Refresh-SessionPath

        # Verify if install succeeded immediately
        if (Get-Command cmake -ErrorAction SilentlyContinue) {
            Write-Success "CMake successfully installed and loaded into this session."
        } else {
            Write-Warning "CMake installed via winget, but you must restart your terminal to use it."
        }
    } else {
        Write-Warning "Please install CMake manually: https://cmake.org/download/"
    }
}

# ============================================================
# 4. Check for C++ Compiler (Visual Studio, MinGW, or Clang)
# ============================================================
Write-Info "Checking for C++ compiler..."
$hasCompiler = $false

# Check for Visual Studio WITH C++ Workload (Safe approach)
# We find vswhere dynamically via standard system variables
$vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswherePath)) {
    $vswherePath = Get-Command vswhere.exe -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Source
}

if ($vswherePath -and (Test-Path $vswherePath)) {
    # -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ensures C++ compiler package is actually installed
    $vsPath = & $vswherePath -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
    if ($vsPath) {
        Write-Success "Visual Studio (with C++ Toolset) found at $vsPath"
        $hasCompiler = $true
    }
}

# Check for MinGW G++
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

# Final Validation
if (-not $hasCompiler) {
    Write-ErrorMsg "No functional C++ compiler (MSVC, G++, Clang) found!"
    Write-Host "  Options to resolve:"
    Write-Host "    1. Install Visual Studio Build Tools: https://visualstudio.microsoft.com/downloads/"
    Write-Host "    2. Install MinGW-w64: https://www.mingw-w64.org/"
    Write-Host "    3. Install via winget: 'winget install Microsoft.VisualStudio.BuildTools'"
}

# ============================================================
# 5. Setup vcpkg for C++ libraries
# ============================================================

$vcpkgRoot = $null

# --- Step 1: Check existing Environment Variables ---
if ($env:VCPKG_ROOT -and (Test-Path $env:VCPKG_ROOT)) {
    $vcpkgRoot = $env:VCPKG_ROOT
    Write-Success "Using existing vcpkg via VCPKG_ROOT: $vcpkgRoot"
} 
# --- Step 2: Check standard installation locations ---
else {
    $standardPaths = @(
        "$env:LOCALAPPDATA\vcpkg",
        "C:\vcpkg",
        "D:\vcpkg",
        "C:\tools\vcpkg",
        "$env:SystemDrive\src\vcpkg"
    )
    
    foreach ($path in $standardPaths) {
        if (Test-Path "$path\.vcpkg-root") { # .vcpkg-root file guarantees it is a real vcpkg folder
            $vcpkgRoot = $path
            Write-Success "Found existing vcpkg at standard path: $vcpkgRoot"
            break
        }
    }
}

# --- Step 3: Check if vcpkg is already available in system PATH ---
if (-not $vcpkgRoot) {
    $vcpkgExe = Get-Command vcpkg -ErrorAction SilentlyContinue
    if ($vcpkgExe) {
        $vcpkgRoot = Split-Path (Split-Path $vcpkgExe.Path)
        Write-Success "Found vcpkg executable in system PATH: $vcpkgRoot"
    }
}

# --- Step 4: Clone as a last resort if completely missing ---
if (-not $vcpkgRoot) {
    $vcpkgRoot = "$env:LOCALAPPDATA\vcpkg"
    Write-Warning "vcpkg not found anywhere. Cloning a fresh copy to $vcpkgRoot..."
    
    # Ensure git is installed before cloning
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
        Write-ErrorMsg "Git is required to install vcpkg, but it was not found on this system."
        exit 1
    }
    
    git clone https://github.com/microsoft/vcpkg.git $vcpkgRoot
    Write-Success "vcpkg cloned to $vcpkgRoot"
}

# --- Step 5: Automatically build/bootstrap vcpkg if executable is missing ---
$vcpkgExecutablePath = Join-Path $vcpkgRoot "vcpkg.exe"
if (-not (Test-Path $vcpkgExecutablePath)) {
    Write-Info "vcpkg.exe is missing. Running the bootstrap script..."
    Push-Location $vcpkgRoot
    try {
        # Run the official bootstrap batch script
        Start-Process -FilePath ".\bootstrap-vcpkg.bat" -Wait -NoNewWindow
        Write-Success "vcpkg bootstrapped successfully."
    } finally {
        Pop-Location
    }
}

# --- Step 6: Export environment variables to ensure persistence ---
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", $vcpkgRoot, "User")
$env:VCPKG_ROOT = $vcpkgRoot

Write-Success "vcpkg is fully ready at: $vcpkgRoot"

# --- Step 7: Install required vcpkg packages ---
if (-not $SkipVcpkg) {
    Write-Info "Installing nlohmann-json..."
    & $vcpkgExecutablePath install nlohmann-json:x64-windows
    Write-Success "nlohmann-json installed."
    
    Write-Info "Installing curl..."
    & $vcpkgExecutablePath install curl:x64-windows
    Write-Success "curl installed."
    
    Write-Success "vcpkg libraries installed."
} else {
    Write-Warning "Skipping vcpkg package installation. You'll need to provide nlohmann-json and curl manually."
}

# ============================================================
# 6. Check for Qt6
# ============================================================

$QtBinPath = $null

# Step 0: Fast check of the QTDIR environment variable
$qtEnv = [Environment]::GetEnvironmentVariable("QTDIR", "User")
if (-not $qtEnv) {
    $qtEnv = [Environment]::GetEnvironmentVariable("QTDIR", "Machine")
}

# If the environment variable exists and points to a valid path, use it
if ($qtEnv -and (Test-Path $qtEnv)) {
    Write-Success "Qt6 found via QTDIR variable: $qtEnv"
    $QtBinPath = $qtEnv
} 
else {
    # If the variable is missing or invalid, start the smart automated search
    Write-Warning "QTDIR environment variable is not configured. Searching for Qt..."
    Write-Info "This may take a moment..."

    $foundPaths = [System.Collections.Generic.List[string]]::new()

    # --- Step 1: Find Qt6 via Windows Uninstaller (Official Installer Registry) ---
    $regPaths = @(
        "HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*",
        "HKLM:\Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*",
        "HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*"
    )

    foreach ($reg in $regPaths) {
        if (Test-Path (Split-Path $reg)) {
            $apps = Get-ItemProperty -Path $reg -ErrorAction SilentlyContinue | 
                    Where-Object { $_.DisplayName -like "*Qt*" -or $_.Publisher -like "*The Qt Company*" }
            
            foreach ($app in $apps) {
                if ($app.InstallLocation -and (Test-Path $app.InstallLocation)) {
                    $foundPaths.Add($app.InstallLocation)
                }
            }
        }
    }

    # --- Step 2: Check standard installation folders ---
    $commonDirs = @("C:\Qt", "D:\Qt", "E:\Qt", "$env:ProgramFiles\Qt", "$env:ProgramFiles(x86)\Qt")
    foreach ($dir in $commonDirs) {
        if (Test-Path $dir) {
            $foundPaths.Add($dir)
        }
    }

    # Remove duplicates from root paths
    $uniqueRootPaths = $foundPaths | Select-Object -Unique

    # Search for qmake.exe inside the detected root directories (Fast, limited depth)
    $finalPaths = [System.Collections.Generic.List[string]]::new()
    foreach ($root in $uniqueRootPaths) {
        Get-ChildItem -Path $root -Filter "qmake.exe" -Recurse -Depth 4 -ErrorAction SilentlyContinue | ForEach-Object {
            # Validate it's Qt6, not Qt5
            $qmakePath = $_.FullName
            $qtVersion = & $qmakePath -query QT_VERSION 2>$null
            if ($qtVersion -and $qtVersion.StartsWith("6.")) {
                $finalPaths.Add($_.DirectoryName)
            }
        }
    }

    # --- Step 3: Deep scan (Last resort for custom/portable extractions) ---
    if ($finalPaths.Count -eq 0) {
        Write-Warning "Qt6 was not found in standard locations. A full disk scan is starting..."
        Write-Warning "This may take several minutes. Press Ctrl+C to skip."
        
        # Detect all local hard drives (C:, D:, etc.)
        $disks = Get-CimInstance -ClassName Win32_LogicalDisk | Where-Object { $_.DriveType -eq 3 } | Select-Object -ExpandProperty DeviceID
        
        foreach ($disk in $disks) {
            $diskPath = $disk + "\"
            Write-Info "Scanning $diskPath..."
            # Recursively scan for qmake.exe, skipping heavy OS folders for performance
            Get-ChildItem -Path $diskPath -Filter "qmake.exe" -Recurse -ErrorAction SilentlyContinue | 
                Where-Object { $_.FullName -notlike "*\Windows\*" -and $_.FullName -notlike "*\$Recycle.Bin\*" } |
                ForEach-Object {
                    $qmakePath = $_.FullName
                    $qtVersion = & $qmakePath -query QT_VERSION 2>$null
                    if ($qtVersion -and $qtVersion.StartsWith("6.")) {
                        $finalPaths.Add($_.DirectoryName)
                    }
                }
        }
    }

    # Extract unique final binary directories
    $uniqueQtPaths = $finalPaths | Select-Object -Unique

    if ($uniqueQtPaths) {
        # Select the first detected valid path for execution
        $QtBinPath = $uniqueQtPaths[0]
        Write-Success "Qt6 automatically located at: $QtBinPath"
    }
}

# Final verification and environment setup
if (-not $QtBinPath) {
    Write-Warning "Qt6 was not found on this computer."
    Write-Host "  Please install Qt6 manually:" -ForegroundColor Cyan
    Write-Host "    - Qt Online Installer: https://qt.io" -ForegroundColor Cyan
    Write-Host "    - Component Choice: Select Qt 6 with MinGW or MSVC toolchains" -ForegroundColor Cyan
    Write-Host "  After installation, set the QTDIR variable or leave it in the default C:\Qt directory." -ForegroundColor Cyan
} else {
    # Set QTDIR for this session and permanently
    [Environment]::SetEnvironmentVariable("QTDIR", $QtBinPath, "User")
    $env:QTDIR = $QtBinPath
    Write-Success "QTDIR set to: $QtBinPath"
    Write-Host "Proceeding using path: $QtBinPath" -ForegroundColor Gray
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
Write-Host "     cmake --preset windows-release"
Write-Host "     cmake --build build/windows-release --preset windows-release"
Write-Host ""
Write-Host "Note: TUI is disabled on Windows, only CLI is supported."

if (-not $hasCompiler) {
    Write-Warning "No C++ compiler found. Install one before building."
}
if (-not $QtBinPath) {
    Write-Warning "Qt6 not found. Install Qt6 and re-run this script."
}

