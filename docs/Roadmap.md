# 🗺️ Rwal – Future Development Roadmap

**Vision:** A lightweight, cross‑platform, user‑friendly wallpaper manager with CLI and TUI, intelligent offline handling, and a robust testing suite.

---

## 🚀 Phase 1 – Cross‑Platform & Core Stability

- [x] **Abstract wallpaper setting** – Create `IWallpaperSetter` interface with implementations for KDE, GNOME and etc.
- [x] **Unit tests** – Add tests for critical modules: keyword parsing, menu navigation, file operations. Use GoogleTest.
- [x] **Beta release**

## 🎨 Phase 2 – Packaging & Addication functional

- [x] **Offline fetching image** – Fetch wallpapers from local storage if network is unavailable.
- [x] **Packaging** – Provide native packages (deb, rpm, exe, dmg).
- [ ] **Config validator** - Create a config validator that checks for syntax errors and compatibility with the config schema.
- [ ] **Release 1.0 version** – First production release.

## 🧪 Phase 3 – Advanced Features & Testing

- [ ] **History browser** – Allow view history of previously selected wallpapers.
- [ ] **Configuration enhancements** – Allow user to specify cache size, preferred image types, and fallback behaviour.
- [ ] **Integration tests** – Simulate network failures, missing config, etc.

---

*Note: Priorities, milestones, and goals may vary depending on the user's feedback.*
