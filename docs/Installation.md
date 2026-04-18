# 🛠 Prerequisites & Installation

## ⚙️ Installation of dependencies

To automatically install all required dependencies (compiler, CMake, Qt5, ncurses, curl, etc.), use the provided `setup.sh` script. It detects your distribution and desktop environment to install the correct packages.

```bash
# Clone the repository 
git clone https://github.com/Aloncie/Rwal

# Or clone with tests( for developers )
git clone --recursive https://github.com/Aloncie/Rwal

cd Rwal

# Give the permission to file
chmod +x setup.sh

# Run
sudo ./setup.sh
```

---

## 🔨 Standard Build Process

```bash
# Configure the project

cmake -B build 

# Build the project
cmake --build build -j$(nproc)
```

## 🔧 Build Configuration Flags

Rwal supports several CMake options to customize your build:

| Flag | Default | Purpose |
|------|---------|---------|
| `-DBUILD_TUI` | `ON` | Build with ncurses Terminal UI support (Linux/macOS only) |
| `-DBUILD_CLI` | `ON` | Build with command‑line interface support |
| `-DRWAL_FORCE_DE` | (auto) | Force a specific desktop environment. See [Supported DE](https://github.com/Aloncie/Rwal/blob/main/docs/SupportedDE.md) |
| `-DBUILD_TESTING`| `OFF` | Build unit tests (requires `--recursive` clone) |

### Examples

```bash
# Build CLI‑only (no ncurses dependency)
cmake -B build -DBUILD_TUI=OFF

# Build TUI‑only (minimal binary)
cmake -B build -DBUILD_CLI=OFF

# Build with tests for development
cmake -B build -DBUILD_TESTING=ON

# Build with force DE
cmake -B build -DRWAL_FORCE_DE=GNOME
```

