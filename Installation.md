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
# Flags:
# (Add -DBUILD_TESTING=ON if you want to run tests)
# (Add -DRWAL_FORCE_DE= if you want to build for forced DE. Check supported DE before using)

cmake -B build 

# Build the project
cmake --build build -j$(nproc)
```

**For documentation, see [DOCUMENTATION.md](https://github.com/Aloncie/Rwal/blob/main/docs/DOCUMENTATION.md).**
