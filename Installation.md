# 🚀 Installation & Quick Start

# 🛠 Prerequisites & Installation

## 1. Install Dependencies by Distribution

### Packages for every DE:

#### KDE

| Distribution | Installation Command |
| :--- | :--- |
| **Fedora** | `sudo dnf install -y git cmake gcc-c++ qt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel` |
| **Ubuntu / Debian** | `sudo apt update && sudo apt install -y git cmake g++ qtbase5-dev libncurses5-dev libcurl4-openssl-dev libnlohmann-json-dev` |
| **Arch Linux** | `sudo pacman -S --needed git cmake gcc qt5-base ncurses curl nlohmann-json` |
| **openSUSE** | `sudo zypper install git cmake gcc-c++ libqt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel glib2-devel` |
| **Gentoo** | `sudo emerge --ask dev-vcs/git dev-util/cmake dev-qt/qtbase sys-libs/ncurses net-misc/curl dev-cpp/nlohmann_json` |
| **CentOS / RHEL 9** | `sudo dnf install -y git cmake gcc-c++ qt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel` |
| **Alpine Linux** | `sudo apk add git cmake g++ qt5-qtbase-dev ncurses-dev curl-dev nlohmann-json` |
| **Solus** | `sudo eopkg install git cmake gcc qt5-base-devel ncurses-devel libcurl-devel nlohmann-json` |
| **Void Linux** | `sudo xbps-install -S git cmake base-devel qt5-devel ncurses-devel libcurl-devel nlohmann-json` |
| **FreeBSD** | `sudo pkg install git cmake gcc qt5-base ncurses curl nlohmann-json` |


---

### 2. Standard Build Process

```bash
# Clone the repository 
git clone https://github.com/Aloncie/Rwal

# Or clone with tests( for developers )
git clone --recursive https://github.com/Aloncie/Rwal

cd Rwal

# Configure the project
# (Add -DBUILD_TESTING=ON if you want to run tests)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build -j$(nproc)

cd build

# Run Rwal interactive mode
./rwal
# Run Rwal change mode
./rwal -c

# for simple 'rwal' and 'rwal -c' anywhere( add in /bin)
sudo make install
```
**For full documentation, see [DOCUMENTATION.md](https://github.com/Aloncie/Rwal/blob/main/docs/DOCUMENTATION.md).**
