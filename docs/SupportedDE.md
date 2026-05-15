# 🖥️ Supported Desktop Environments

Rwal supports multiple desktop environments through compile‑time‑selected adapters.  
The following setters are implemented and maintained.

---

## ✅ Officially Supported (compile‑time selectable)

| Desktop Environment | Setter Class | Detection / Dependencies | Notes |
|---------------------|--------------|--------------------------|-------|
| **GNOME** | `GnomeSetter` | `XDG_CURRENT_DESKTOP` contains `GNOME`; uses GIO/GSettings | Sets both `picture-uri` and `picture-uri-dark` |
| **KDE Plasma** | `KdeSetter` | D‑Bus call to `org.kde.plasmashell` | Uses `evaluateScript` with JavaScript to set wallpaper per desktop |
| **Hyprland** | `HyprlandSetter` | `HYPRLAND_INSTANCE_SIGNATURE` env var; tools: `hyprpaper` or `swww` | Falls back to `hyprpaper` → `swww` if available |
| **Windows** | `WindowsSetter` | `windows.h`| Use `SPI_SETDESKWALLPAPER` with `SetSystemDirectory` to set the wallpaper` |
| **Fallback** | `FallbackSetter` | Always available (if no DE matches) | Logs an error and returns `false` – no wallpaper change |

> **Your distribution not listed? Open an issue – we'd love to hear about your setup!**
> **[Request a setter](https://github.com/Aloncie/Rwal/discussions)**

---

## 🔧 Build Configuration

Rwal use auto-detect system for know DE to build only needly sources. This gives:
- more faster building
- takes up less space

You can use special flag ```-DRWAL_FORCE_DE=```, If Rwal incorrect detect your DE. Flag command supports any writing case.
Example: ```-DRWAL_FORCE_DE=KDE``` or ```-DRWAL_FORCE_DE=kde```

[See more about configuration flags](https://github.com/Aloncie/Rwal/blob/refactorKnownIssues/docs/Installation.md#-build-configuration-flags).

## 🐧 Runtime Behaviour

*   **The compiled setter is used unconditionally** – no runtime auto‑detection across different DEs.
*   **Environment Mismatch:** If you run a GNOME‑built binary on KDE, it will likely fail. (The `GnomeSetter` checks `XDG_CURRENT_DESKTOP` and may reject the environment, returning `false`).
*   **Sanity Checks:** Each setter performs its own validation (environment variables, required tools, file existence) and logs failures.

--- 

## 📁 Code Locations

```text
src/wallpaper/
├── CMakeLists.txt                # Main logic
├── IWallpaperSetter.hpp          # Interface
├── WallpaperFactory.cpp          # Conditional inclusion logic
├── platforms/
│   ├── GnomeSetter.hpp/cpp
│   ├── KdeSetter.hpp/cpp
│   ├── HyprlandSetter.hpp/cpp
│   ├── WindowsSetter.hpp/cpp
│   ├── FallbackSetter.hpp/cpp
```

## 🧪 Tested Distributions (by setter)


| Setter | Tested on |
| :--- | :--- |
| **GnomeSetter** | Fedora 40+, Ubuntu 24.04+, Arch Linux (GNOME) |
| **KdeSetter** | Arch Linux (KDE)|
| **HyprlandSetter** | not tested |
| **WindowsSetter** | not tested |

> **Are there problems with one of DE?**
> **[Report an issue](https://github.com/Aloncie/Rwal/issues)**

---

## 🚧 Limitations & Future Work

*   **No runtime DE switching** – the binary is tied to one DE at compile time.
*   **Multi‑monitor** is handled by the underlying desktop API.
*   **Wayland/X11** – GNOME and KDE setters work under both; Hyprland is Wayland‑only.

<div align="center">

### Rwal – one build, one desktop, reliable wallpaper switching. 🐧

</div>


