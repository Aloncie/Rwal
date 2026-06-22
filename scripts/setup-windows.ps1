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
  1. Checks for Git, CMake, vcpkg and C++ compiler.
  2. Installs missing tools via winget (or shows manual URLs)
  3. Bootstraps vcpkg for C++ libraries
  4. Installs nlohmann-json and curl via vcpkg
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
# 4. Check for Ninja
# ============================================================
Write-Info "Checking for Ninja..."
$ninjaPath = Get-Command ninja -ErrorAction SilentlyContinue
if ($ninjaPath) {
    $ninjaVersion = (ninja --version) 2>$null
    Write-Success "Ninja $ninjaVersion found at $($ninjaPath.Source)"
} else {
    Write-Warning "Ninja not found."
    if (-not $SkipWinget -and (Get-Command winget -ErrorAction SilentlyContinue)) {
        Write-Info "Installing Ninja via winget..."
        winget install -e --id Ninja-build.Ninja
        Write-Success "Ninja installed."
    } else {
        Write-Warning "Please install Ninja manually: https://ninja-build.org/"
    }
}

# ============================================================
# 5. Check for C++ Compiler (Visual Studio, MinGW, or Clang)
# ============================================================
Write-Info "Checking for C++ compiler..."

# Helper: try to run a compiler and return its version string
function Test-CompilerVersion {
    param([string]$ExePath, [string]$Flag = "--version")
	# Check for Visual Studio compiler
    if ($ExePath -like "*\cl.exe") {
        return Test-Path $ExePath
    }
	# Any other compiler
    try {
        $output = & $ExePath $Flag 2>&1 | Select-Object -First 1
        return $output
    } catch { return $null }
}

$hasCompiler = $false
$foundCompilers = [System.Collections.Generic.List[string]]::new()

# ================================================================
# Echelon 1 – PATH (fast, zero disk I/O)
# ================================================================
foreach ($cmd in @("g++", "clang++")) {
    $p = Get-Command $cmd -ErrorAction SilentlyContinue
    if ($p) {
        $ver = Test-CompilerVersion $p.Source
        Write-Success "$cmd found on PATH: $ver"
        $hasCompiler = $true
        $foundCompilers.Add($p.Source)
    }
}

# ================================================================
# Echelon 2 – vswhere for official MSVC installs
# ================================================================
$vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswherePath)) {
    $vswherePath = Get-Command vswhere.exe -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Source
}

if ($vswherePath -and (Test-Path $vswherePath)) {
    # Get ALL installations, then verify cl.exe manually
    $vsPaths = & $vswherePath -products * -property installationPath 2>$null
	# Look inside every folder and find the cl.exe
    foreach ($vsRoot in $vsPaths) {
        if (-not $vsRoot) { continue }
        $clCandidates = Get-ChildItem -Path "$vsRoot\VC\Tools\MSVC" -Filter "cl.exe" -Recurse -Depth 8 -ErrorAction SilentlyContinue |
                        Where-Object { $_.FullName -like "*Hostx64\x64*" } |
                        Select-Object -First 1
        if ($clCandidates) {
			Write-Success "MSVC found via vswhere: $($clCandidates.FullName)"
			$hasCompiler = $true
			$foundCompilers.Add($clCandidates.FullName)
			break # first working MSVC is enough
        }
    }
}

# ================================================================
# Echelon 3 – Smart filesystem scan (portable / unregistered installs)
# ================================================================
if (-not $hasCompiler) {
    Write-Info "No compiler on PATH or vswhere. Scanning known locations on all drives..."

    # Compiler search map: executable name -> folder names where it typically lives
    $compilerMap = @{
        "g++.exe" = @("MinGW", "mingw64", "mingw32", "msys64", "msys2", "CodeBlocks")
        "clang++.exe" = @("LLVM", "llvm", "clang")
        "cl.exe" = @("Microsoft Visual Studio", "BuildTools", "VC")
    }

    # Get all fixed (non-removable) drives
	# DriveType 3 = local hard disk
    $drives = Get-CimInstance -ClassName Win32_LogicalDisk |
              Where-Object { $_.DriveType -eq 3 } |
              Select-Object -ExpandProperty DeviceID

    # Additional root locations on the system drive
    $extraRoots = @(
        "${env:ProgramFiles}", # C:\Program Files
        "${env:ProgramFiles(x86)}", # C:\Program Files (x86)
        "$env:USERPROFILE", # C:\Users\username
		"$env:LocalAppData", # C:\Users\username\AppData\Local
        "$env:SystemDrive\" # C:\
    )

    # Collect all unique search roots
    $searchRoots = [System.Collections.Generic.HashSet[string]]::new()
    foreach ($d in $drives) { $searchRoots.Add("$d\") | Out-Null }
    foreach ($r in $extraRoots) { if ($r) { $searchRoots.Add($r) | Out-Null } }
		
	# Look inside every folder and find the compiler
    :compilerSearch foreach ($root in $searchRoots) {
        if (-not (Test-Path $root)) { continue }
        foreach ($exeName in $compilerMap.Keys) {
            $folderNames = $compilerMap[$exeName]
            foreach ($folder in $folderNames) {
                $candidateDirs = Get-ChildItem -Path $root -Directory -Filter $folder -Depth 2 -ErrorAction SilentlyContinue
                foreach ($dir in $candidateDirs) {
                    $found = Get-ChildItem -Path $dir.FullName -Filter $exeName -Recurse -Depth 5 -ErrorAction SilentlyContinue |
                             Select-Object -First 1
                    if ($found) {
                        $ver = Test-CompilerVersion $found.FullName
                        if ($ver) {
                            Write-Success "$exeName found via scan: $($found.FullName)"
                            $hasCompiler = $true
                            $foundCompilers.Add($found.FullName)
                            break compilerSearch # one working compiler is enough
                        }
                    }
                }
            }
        }
    }
}

# Final verdict
if (-not $hasCompiler) {
    Write-ErrorMsg "No functional C++ compiler (MSVC, G++, Clang) found!"
    Write-Host "  Options to resolve:"
    Write-Host "    1. Install Visual Studio Build Tools: https://visualstudio.microsoft.com/downloads/"
    Write-Host "    2. Install MinGW-w64: https://www.mingw-w64.org/"
    Write-Host "    3. Install via winget: 'winget install Microsoft.VisualStudio.BuildTools'"
    Write-Host "    4. Add your compiler to the system PATH and re-run this script."
}

# ============================================================
# 6. Setup vcpkg for C++ libraries
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
# 7. Summary
# ============================================================

Write-Host @"
========================================
   Setup Complete
========================================
"@ -ForegroundColor Magenta

Write-Host "Next steps:"
Write-Host "  1. Restart your terminal"
Write-Host "  2. Build Rwal via build.py"
Write-Host ""
Write-Host "Note: TUI is disabled on Windows, only CLI is supported."

if (-not $hasCompiler) {
    Write-Warning "No C++ compiler found. Install one before building."
}
