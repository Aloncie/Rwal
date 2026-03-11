#!/bin/bash
echo "🔨 Building Rwal..."
cd /app
mkdir -p build && cd build
cmake .. && make -j$(nproc)

echo "🚀 Running: $@"
exec "$@"  # This runs whatever command was passed (like ./rwal or bash)
