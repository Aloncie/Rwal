#!/usr/bin/env python3
"""
setup.py — Cross-platform installer for Rwal
Usage: python setup.py
"""

import sys
import os
import subprocess

def run_script(command, shell=False):
    """Safely runs an external script or command."""
    try:
        result = subprocess.run(command, shell=shell, check=True)
        return result.returncode == 0
    except subprocess.CalledProcessError as e:
        print(f"\nError: Script failed with exit code {e.returncode}")
        return False
    except FileNotFoundError:
        print(f"\nError: Could not find the target script file or interpreter.")
        return False

def main():
    print("Rwal Dependency Setup")
    print("=====================")
    
    platform = sys.platform
    
    # Windows Detection
    if platform in ("win32", "cygwin"):
        print("Windows detected. Starting Windows setup...")
        # Path to your existing PowerShell script
        ps_script = os.path.join("scripts", "setup-windows.ps1")
        
        # Bypass execution policy safely for this single run
        cmd = ["powershell.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", ps_script]
        success = run_script(cmd)
        sys.exit(0 if success else 1)
        
    # macOS Detection
    elif platform == "darwin":
        print("macOS detected.")
        print("Setup script for macOS is not yet available.")
        print("Please install dependencies manually: git, cmake, qt6, curl, nlohmann-json, ncurses")
        sys.exit(0)
        
    # Linux Detection
    elif platform.startswith("linux"):
        print("Linux detected. Starting Linux setup...")
        # Path to your existing bash script
        linux_script = os.path.join("scripts", "setup-linux.sh")
        
        cmd = ["bash", linux_script]
        success = run_script(cmd)
        sys.exit(0 if success else 1)
        
    # Unknown OS Fallback
    else:
        print(f"Unknown OS: {platform}")
        print("Please install dependencies manually: git, cmake, qt6, curl, nlohmann-json, ncurses")
        sys.exit(1)

if __name__ == "__main__":
    main()

