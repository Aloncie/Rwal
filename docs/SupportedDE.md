# 🖥️ Supported Desktop Environments

Rwal supports multiple desktop environments through runtime-time-selected setter.  
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

## 🐧 Runtime Behaviour
*   **Binary has multiple setters** – the setter is selected at runtime based on the detected DE.
*   **Sanity Checks:** Each setter performs its own validation (environment variables, required tools, file existence) and logs failures.

--- 

## 📁 Code Locations

```text
src/wallpaper/
├── CMakeLists.txt                # CMake logic
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
| **WindowsSetter** | Windows 11 25H2 |

> **Are there problems with one of DE?**
> **[Report an issue](https://github.com/Aloncie/Rwal/issues)**

---

<div align="center">

### Rwal – one build, any desktop, reliable wallpaper switching. 🐧

</div>
