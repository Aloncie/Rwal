# 🏛️ Rwal: Architecture and Philosophy

[Aloncie](https://github.com/Aloncie):
> Rwal is first and foremost a **learning project** for me. But I have been triyng upgrade Rwal. This project always will be in my heart.
> 
> It's not a commercial product, but it's built with professional intent – every line is a lesson.


---

## 🎯 Vision

Rwal is a **mood‑management tool**. It doesn't just set pretty pictures – it manages the visual context of your desktop.

- **Modern c++:** Use the power of Qt, thread, smart unique.
- **Hybrid run:** Two modes – interactive configuration (Core) and silent daemon‑like operation (Change).

Unlike heavy all‑in‑one tools (like Wallpaper Engine), Rwal aims to be:
1. **Lightweight** – written in C++, minimal RAM usage (no Electron/JS).
2. **Smart** – search by tags, filters, colour, resolution.
3. **Modular** – clear separation of concerns.

---

## 🛠 Interaction Methods

The tool is available in two flavors:
* **Graphical User Interface (GUI):** Ideal for visual configuration and quick starts.
* **Command Line Interface (CLI):** Built for speed, scripting, and headless environments.

---

## 👾 Modes of Operation

### Core Mode
Full interactive terminal UI. The user can:
- Navigate menus
- Edit keywords (opens `$EDITOR`)
- Configure the systemd timer
- Save the current wallpaper to `~/Pictures/rwal`

### Change Mode
Maximum reliability, minimal feedback. Designed for cron/systemd timers.
- Takes a random keyword from the config.
- If no keywords, falls back to defaults.
- If offline, uses a previously downloaded image.
- No console output; logs go to `logs.txt`.

---

## 🏗️ Architecture

The project is built with clear separation of responsibilities.

### Key Design Patterns

1. **Singleton** – used for `Config`, `Logs`, `MenuManager`, `NetworkManager` to provide global access. (May be refactored later for testability.)
2. **Observer** – `QFileSystemWatcher` watches `config.json`; changes are hot‑reloaded.
3. **Dependency Injection (lite)** – `Navigator` receives a pointer to the current menu, allowing easy screen switching.

