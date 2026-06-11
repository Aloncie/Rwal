# 🏛️ Rwal: Architecture and Philosophy

[Aloncie](https://github.com/Aloncie):
> Rwal is first and foremost a **learning project** for me. I am constantly evolving it, and every line of code serves as a lesson in professional software development.
> 
> It's not a commercial product, but it's built with professional intent — every line is a lesson.

---

## 🎯 Vision

Rwal is a **mood‑management tool**. It doesn't just set pretty pictures – it manages the visual context of your desktop.

- **Modern C++:** Leveraging Qt, threading, and modern memory management (RAII, smart pointers like `std::unique_ptr`).
- **Hybrid execution:** Two modes – interactive configuration (**Core**) and silent daemon‑like operation (**Change**).

Unlike heavy all‑in‑one tools (like Wallpaper Engine), Rwal aims to be:
1. **Lightweight** – written in C++, minimal RAM usage (no Electron/JS).
2. **Smart** – search by tags, filters, color, resolution.
3. **Modular** – clear separation of concerns.

---

## 🛠 Interaction Methods

Rwal is evolving to support three interaction tiers:
* **Command Line Interface (CLI):** Built for speed, scripting, and headless environments.
* **Terminal Interface (TUI):** The minimalistic, console‑only interface for quick, manual control.
* **Graphical User Interface (GUI):** (Planned) Ideal for visual configuration and ease of use.

---

## 👾 Modes of Operation

### Core Mode
Full interactive terminal UI (TUI). The user can:
- Navigate menus
- Edit keywords (integrates with your system `$EDITOR`; falls back to `nano` if unset)
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

1. **Singleton (Transitioning):** `Config` and `Logs` are currently managed as single instances. The codebase is moving toward **Dependency Injection** for better testability.
2. **Observer:** `QFileSystemWatcher` watches `config.json`; changes trigger hot‑reload of configuration.
3. **Abstraction:** Interfaces `IUserInterface` and `IWallpaperSetter` decouple core logic from platform‑specific UI and desktop environment code.

