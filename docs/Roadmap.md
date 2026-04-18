# 🗺️ Rwal – Future Development Roadmap

**Vision:** A cross‑platform, user‑friendly wallpaper manager with CLI, TUI and GUI(planned), intelligent offline handling, and a robust testing suite.

---

## 🚀 Phase 1 – Cross‑Platform & Core Stability

- [x] **Abstract wallpaper setting** – Create `IWallpaperSetter` interface with implementations for KDE, GNOME and etc.
- [ ] **Offline fallback** – Maintain a local cache of recently downloaded wallpapers; if offline, pick a random cached image.
- [ ] **Configuration enhancements** – Allow user to specify cache size, preferred image types, and fallback behaviour.
- [ ] **Unit tests** – Add tests for critical modules: keyword parsing, menu navigation, file operations. Use GoogleTest or Qt Test.

## 🎨 Phase 2 – GUI & Addication functional

- [ ] **Basic GUI mode** – Develop a simple Qt Widgets interface (non‑QML) for users who prefer a window. Keep CLI as first‑class citizen.
- [ ] **System tray integration** – Allow quick wallpaper refresh and mode switching from tray icon.
- [ ] **History browser** – GUI to view and restore previously used wallpapers.
- [ ] **Beta release**

## 🧪 Phase 3 – Testing & Release

- [ ] **Integration tests** – Simulate network failures, missing config, etc.
- [ ] **Packaging** – Provide native packages (deb, rpm, AppImage, Windows installer, macOS .dmg).
- [ ] **Stable 1.0** – First production release.

---

*Note: Priorities, milestones, and goals may vary depending on the user's feedback.*
