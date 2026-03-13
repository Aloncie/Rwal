#!/bin/bash
set -e  # Exit on any error

echo "🔨 Building Rwal in Docker..."

if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: CMakeLists.txt not found in $(pwd)"
    echo "Make sure you're mounting the project root to /app"
    exit 1
fi

mkdir -p build && cd build

echo "⚙️ Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

echo "🏗️ Building..."
make -j$(nproc)

echo "✅ Build complete!"

if [ $# -gt 0 ]; then
    echo "🚀 Running: $@"
    exec "$@"
else
    echo "💡 No command provided. The binary is at ./build/rwal"
fi
