#!/bin/bash
# setup.sh - Rwal Setup Dispatcher
# Run: ./setup.sh

echo "Rwal Dependency Setup"
echo "====================="

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    echo "Windows detected."
    echo "Please run the PowerShell script:"
    echo "  powershell -ExecutionPolicy Bypass -File scripts/setup-windows.ps1"
    exit 0
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macOS detected."
    echo "Setup script for macOS is not yet available."
    echo "Please install dependencies manually: git, cmake, qt6, curl, nlohmann-json"
    exit 0
else
    echo "Linux detected. Starting Linux setup..."
    exec bash scripts/setup-linux.sh
fi
