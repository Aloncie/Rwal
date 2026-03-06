<h1 align="center">Hi there, I'm <a href="https://github.com/Aloncie" target="_blank">Aloncie</a> <img src="https://raw.githubusercontent.com/MartinHeinz/MartinHeinz/master/wave.gif" width="30px"></h1>

<h3 align="center">👾 Today I'm glad to present my new(first) project... 👾</h3>

<br>

# 🌊 Rwal – Random Wallpaper Utility

![Version](https://img.shields.io/badge/version-alpha-red)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue?logo=c%2B%2B)
![CMake](https://img.shields.io/badge/CMake-3.10%2B-purple?logo=cmake)
![Qt5](https://img.shields.io/badge/Qt-5.15-green?logo=qt)
![License](https://img.shields.io/badge/license-MIT-orange)
![Build](https://img.shields.io/badge/build-CMake%20%7C%20Make-yellow)
![Status](https://img.shields.io/badge/status-active-brightgreen)

---

**Rwal** is a hybrid CLI wallpaper utility that combines an interactive terminal interface with a silent, daemon‑like mode for regular wallpaper changes. It fetches images from [Wallhaven](https://wallhaven.cc) based on user‑defined keywords and sets them via D‑Bus on KDE Plasma (with cross‑platform support planned). Built with modern C++20 and Qt5.

---

## ✨ Features

- 🎨 **Interactive menu** – navigate, add/remove keywords, configure timer.
- 🔄 **Silent mode** – one‑shot wallpaper change, ideal for cron/systemd.
- ⚙️ **Hot‑reload config** – edit `config.json` while the app runs.
- 🧠 **Smart keyword handling** – random selection, manual editing via `$EDITOR`.
- 🕒 **Systemd timer integration** – set hourly/daily wallpaper rotation.
- 📝 **Built‑in logging** – with auto‑rotation and permission handling.
- 🧩 **Modular design** – clear separation of UI, networking, and platform layers.

---

## 📦 Dependencies

- Qt5 (Core, DBus, Widgets)
- libcurl
- nlohmann_json
- ncurses
- CMake ≥ 3.10
- C++20 compiler (GCC/Clang)

---

## 🚀 Installation & Quick Start

```bash
git clone https://github.com/Aloncie/Rwal.git
cd Rwal
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install   # optional, installs to /usr/local/bin
```

Run interactively:
```bash
rwal
```

Run silent change (e.g., for a cron job):
``` bash
rwal --change
# or
rwal -c
```

For full documentation, see [DOCUMENTATION.md](https://github.com/Aloncie/Rwal/blob/main/docs/DOCUMENTATION.md).

---

## ⚙️ Configuration (config.json)

Automatically created on first run in ~/.config/Aloncie/rwal/config.json.

``` json
{
    "search": {
        "keywords": [
	        kewords for search
        ],
        "res": resolution wallpaper's,
        "sorting": type of sorting
    },
    "services": {
        "wallhaven": {
            "apikey": "api-key",
            "base_url": "https://wallhaven.cc/api/v1/search",
            "param_names": {
                "query": "?q=",
                "res": "resolutions",
                "sorting": "sorting"
            }
        }
    }
}
```
**Changes are hot‑reloaded – no restart needed.**

## 🛡️ License

This project is licensed under the MIT License – see the LICENSE file for details.

## 🔒 Security

For security issues, please see our [Security Policy](SECURITY.md).

## 📫 Contact
- Author: [@Aloncie](https://github.com/Aloncie)
- Project repo: [Rwal](https://github.com/Aloncie/Rwal)

## 🔗 Links

- [Philosophy](https://github.com/Aloncie/Rwal/blob/main/docs/PHILOSOPHY.md)
- [Documentation](https://github.com/Aloncie/Rwal/blob/main/docs/DOCUMENTATION.md)
- [Roadmap](https://github.com/Aloncie/Rwal/blob/main/docs/ROADMAP.md)

## 📊 2-Minute Survey

Help shape Rwal's future: [google-form](https://forms.gle/JCewzzEUwPGxx7cY7)

**Made with ❤️ and a lot of ☕ by Aloncie.**
