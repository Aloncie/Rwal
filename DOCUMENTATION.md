**DOCUMENTATION.md**
```markdown
# 📘 Rwal – Detailed Documentation

This document complements the README with in‑depth information about the project's architecture, configuration, and usage.

---

## 🏛️ Philosophy

Rwal is designed as a learning vehicle for modern C++ and system programming, while still being a useful tool. It follows the **JETT** principle: use powerful libraries (Qt, libcurl) but avoid unnecessary complexity.

Two distinct modes:
- **Core Mode** – full interactive terminal UI for configuration.
- **Change Mode** – silent, single‑shot wallpaper change, ideal for timers.

For a deeper dive, see [Philosophy](#philosophy) (or the original `RwalPhilosophy.md` in the source tree).

---

## 🔧 Building from Source

See the README for quick build instructions. For advanced options:

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/your/prefix
make
make install

## 🎮 Usage
### 👾Interactive Mode

Launch without arguments:
``` bash
rwal
```
Use the number keys to navigate:

    1 – refresh wallpaper immediately
    2 – save current wallpaper to your Pictures folder
    3 – manage keywords (add, remove, open in $EDITOR)
    4 – settings (timer, wallpaper path)
    q – quit

### 😶Silent Mode

```bash
rwal --change
```

This picks a random keyword from your config, downloads a random image from that search, and sets it as wallpaper. No output is printed – all logs go to logs.txt.

## 📁 Project Structure
text

src/
├── internal/          – utilities (strings, vectors, platform helpers)
├── dbus/              – KDE Plasma D‑Bus integration
├── net/               – libcurl wrappers (MyCurl, NetworkManager)
├── ui/                – CLI menus, Navigator, output coloring
├── logs/              – logging class
├── keywords/          – keyword management (read, edit, random)
├── settings/          – config, timer, pictures path
├── funcs/             – miscellaneous helpers
├── navigator/         – menu state machine
└── wallpaper/         – download, save, set wallpaper

## 📝 Logging

Logs are stored in ~/.cache/rwal/logs.txt. They include timestamps, network requests, errors, and user input. The log file is automatically rotated when it exceeds 1 MB.
🧪 Current Status & Roadmap

See RwalRoadMap.md (in the repository) for the up‑to‑date development plan.
🤝 Contributing

While this is a personal learning project, suggestions and bug reports are welcome via GitHub Issues.
```
