# 💡 Rwal: Ideas & Future Enhancements

## 🏗 Architectural Evolution (McConnell Inspired)
- [ ] **Decouple Network Logic:** Encapsulate `libcurl` entirely. The rest of the system should interact with an `INetworkClient` interface, unaware of CURL-specific buffers or headers.
- [ ] **Eliminate Singleton Logs:** Transition from `Logs::getInstance()` to a Dependency Injection pattern or a static sink-based logger to improve testability.
- [ ] **Value Objects over Strings:** Introduce `Url` and `Path` classes to avoid passing raw strings between `CurlWrapper` and `WallpaperManager`.
- [ ] **Defensive Programming Layer:** Implement a centralized validation layer for configuration files to prevent "garbage-in, garbage-out" scenarios.

## 🛠 Feature Ideas
- [ ] **Cross-Platform Abstraction:** Implement a `PlatformFactory` to hide the "secret" of which OS is currently running (GNOME, Windows, KDE).
- [ ] **Dry-Run Mode:** A flag to simulate wallpaper changes without touching the filesystem/registry (useful for testing complex logic).
- [ ] **Batch Mode:** Add a CLI flag to process multiple keywords in a single run without the interactive UI.
- [ ] **Refactor CurlWrapper:** Remove file-naming logic (suffixes, prefixes) from `CurlWrapper`. 
    - *Action:* Create `IStorageProvider` interface.
    - *Goal:* Hide the "secret" of how files are named on the disk from the networking layer. 

---

