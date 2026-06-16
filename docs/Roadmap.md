# 🗺️ Rwal – Future Development Roadmap

**Vision:** A cross‑platform, user‑friendly wallpaper manager with CLI and TUI, intelligent offline handling, and a robust testing suite.

---

## 🚀 Phase 1 – Cross‑Platform & Core Stability

- [x] **Abstract wallpaper setting** – Create `IWallpaperSetter` interface with implementations for KDE, GNOME and etc.
- [x] **Unit tests** – Add tests for critical modules: keyword parsing, menu navigation, file operations. Use GoogleTest.
- [x] **Beta release**

## 🎨 Phase 2 – Packaging & Addication functional

- [x] **Offline fetching image** – Fetch wallpapers from local storage if network is unavailable.
- [x] **Packaging** – Provide native packages (deb, rpm, exe, dmg).
- [ ] **Configuration enhancements** – Allow user to specify cache size, preferred image types, and fallback behaviour.
- [ ] **History browser** – Allow view history of previously selected wallpapers.

## 🧪 Phase 3 – Testing & Release

- [ ] **Integration tests** – Simulate network failures, missing config, etc.
- [ ] **Stable 1.0** – First production release.

---

*Note: Priorities, milestones, and goals may vary depending on the user's feedback.*
