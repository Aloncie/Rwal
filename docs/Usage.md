## 🚀 Usage

Rwal can be used in two modes: **Interactive Terminal UI** (best for manual control) and **CLI Flags** (best for scripts and automation).

### 🖥 Terminal UI (TUI)
Run Rwal without arguments to enter the interactive menu. It allows you to manage your desktop environment quickly using single-key commands.

```bash
# in Rwal's directory
./build/rwal
```

**Main Menu Controls:**

- `1` : **Refresh** — Fetch and apply a new wallpaper immediately.
- `2` : **Save** — Save the current wallpaper to your collection.
- `3` : **Keywords** — View and manage search tags.
- `4` : **Settings** — Configure application behavior.
- `q` : **Quit** — Safely exit the application.

### ⚡ Command Line Interface (CLI)
Use flags for quick actions or to integrate Rwal into your startup scripts (e.g., `.bashrc`, `.zshrc`, or window manager configs like Hyprland/i3).

**🖼 Wallpaper Management**
- `rwal -c` : Change wallpaper once and exit immediately.
- `rwal -s` : Save the current wallpaper.

**🏷 Keyword Management**
Control what kind of images Rwal fetches:
- `rwal -k` : Show currently active keywords.
- `rwal --set-keywords "nature,ocean"` : Overwrite all keywords.
- `rwal --add-keywords "dark,space"` : Add new tags to the existing list.
- `rwal --remove-keywords "town"` : Remove specific tags.

**📜 Maintenance & Info**
- `rwal --log` : Display the latest logs (with timestamp and module info).
- `rwal --clear-logs` : Wipe the log file to save space.
- `rwal -v` : Show version and build information.
- `rwal -h` : Display the full list of available options.
- `rwal --contact` : Show contact info and links.

### 💡 Pro Tips
- **Automation:** Add `rwal -c` to your OS startup applications to get a fresh wallpaper every time you log in.
- **Logging:** If something goes wrong, check the logs with `rwal --log`. Rwal now uses a high-precision format: `[YYYY-MM-DD | HH:MM:SS]`.
