<h1 align="center">Hi there, I'm <a href="https://github.com/Aloncie" target="_blank">Aloncie</a> <img src="https://raw.githubusercontent.com/MartinHeinz/MartinHeinz/master/wave.gif" width="30px"></h1>

<h3 align="center">👾 Today I'm glad to present my new(first) project... 👾</h3>

<br>

# 🌊 Rwal – Random Wallpaper Utility

![Version](https://img.shields.io/badge/version-alpha-red)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue?logo=c%2B%2B)
![CMake](https://img.shields.io/badge/CMake-3.10%2B-purple?logo=cmake)
![Qt5](https://img.shields.io/badge/Qt-5-yellowgreen?logo=qt)
![License](https://img.shields.io/badge/license-MIT-orange)

---

**Rwal** is a hybrid CLI wallpaper utility that combines an interactive terminal interface with a silent, daemon‑like mode for regular wallpaper changes. It fetches images from [Wallhaven](https://wallhaven.cc). Built with modern C++20 and Qt5.

---

## ✨ Features

- **Interactive menu** – navigate, add/remove keywords, configure timer.
- **Silent mode** – one‑shot wallpaper change, ideal for cron/systemd.
- **Hot‑reload config** – edit `config.json` while the app runs.
- **Smart keyword handling** – random selection, manual editing via `$EDITOR`.
- **Systemd timer integration** – set hourly/daily wallpaper rotation.
- **Built‑in logging** – with auto‑rotation and permission handling.
- **Modular design** – clear separation of UI, networking, and platform layers.

---


## 🚀 Installation & Quick Start

## 🛠 Prerequisites & Installation

### 1. Install Dependencies by Distribution

Select the command based on your Operating System and Desktop Environment (GNOME or KDE):

| Distribution | Environment | Installation Command |
| :--- | :--- | :--- |
| **Fedora** | **GNOME** | `sudo dnf install -y git cmake gcc-c++ qt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel glib2-devel` |
| **Fedora** | **KDE** | `sudo dnf install -y git cmake gcc-c++ qt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel` |
| **Ubuntu / Debian** | **GNOME** | `sudo apt update && sudo apt install -y git cmake g++ qtbase5-dev libncurses5-dev libcurl4-openssl-dev libnlohmann-json-dev libglib2.0-dev` |
| **Ubuntu / Debian** | **KDE** | `sudo apt update && sudo apt install -y git cmake g++ qtbase5-dev libncurses5-dev libcurl4-openssl-dev libnlohmann-json-dev` |
| **Arch Linux** | **Any** | `sudo pacman -S --needed git cmake gcc qt5-base ncurses curl nlohmann-json glib2` |
| **openSUSE** | **Any** | `sudo zypper install git cmake gcc-c++ libqt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel glib2-devel` |

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

---

## 👁️‍🗨️How it works?

### Change mode
![Image](https://github.com/user-attachments/assets/c01c9cea-cbe5-4ddd-8f00-f4731b461fb6)

### Interactive mode(input doesn't print)
![Image](https://github.com/user-attachments/assets/9de86ea7-a8f6-4b4b-8098-25ccc14520bb)
---

## 🛡️ License

This project is licensed under the MIT License – see the LICENSE file for details.

## 🔒 Security

For security issues, please see our [Security Policy](SECURITY.md).

## 📫 Contact
- Author: [@Aloncie](https://github.com/Aloncie)
- Project repo: [Rwal](https://github.com/Aloncie/Rwal)
- Email: [Aloncie@proton.me](mailto:Aloncie@proton.me) 

## 🔗 Links

- [Philosophy](https://github.com/Aloncie/Rwal/blob/main/docs/PHILOSOPHY.md)
- [Documentation](https://github.com/Aloncie/Rwal/blob/main/docs/DOCUMENTATION.md)
- [Roadmap](https://github.com/Aloncie/Rwal/blob/main/docs/ROADMAP.md)
- [Flow Graphs](https://github.com/Aloncie/Rwal/blob/main/docs/FlowGraphs.md)

## 📊 2-Minute Survey

Help shape Rwal's future: [google-form](https://forms.gle/JCewzzEUwPGxx7cY7)

**Made with ❤️ and a lot of ☕ by Aloncie.**
