# 🖥️ Supported Desktop Environments

Rwal supports multiple desktop environments through compile‑time‑selected adapters.  
The following setters are implemented and maintained.

---

## ✅ Officially Supported (compile‑time selectable)

| Desktop Environment | Setter Class | Detection / Dependencies | Notes |
|---------------------|--------------|--------------------------|-------|
| **GNOME** (42+) | `GnomeSetter` | `XDG_CURRENT_DESKTOP` contains `GNOME`; uses GIO/GSettings | Sets both `picture-uri` and `picture-uri-dark` |
| **KDE Plasma** (5.x/6.x) | `KdeSetter` | D‑Bus call to `org.kde.plasmashell` | Uses `evaluateScript` with JavaScript to set wallpaper per desktop |
| **Hyprland** (Wayland) | `HyprlandSetter` | `HYPRLAND_INSTANCE_SIGNATURE` env var; tools: `hyprpaper` or `swww` | Falls back to `hyprpaper` → `swww` if available |
| **Fallback** | `FallbackSetter` | Always available (if no DE matches) | Logs an error and returns `false` – no wallpaper change |

---

## 🔧 Build Configuration

Rwal use auto-detect system for know DE to build only needly sources. This gives:
- more faster building
- takes up less space

You can use special flag ```-DRWAL_FORCED_DE=```, If Rwal incorrect detect your DE. Flag command supports any writing case.

*Example for GNOME:*
```bash
# this command works fine
cmake -B build -DRWAL_FORCED_DE=GNOME

# this too
cmake -B build -DRWAL_FORCED_DE=GnoME
```

## 🐧 Runtime Behaviour

*   **The compiled setter is used unconditionally** – no runtime auto‑detection across different DEs.
*   **Environment Mismatch:** If you run a GNOME‑built binary on KDE, it will likely fail. (The `GnomeSetter` checks `XDG_CURRENT_DESKTOP` and may reject the environment, returning `false`).
*   **Sanity Checks:** Each setter performs its own validation (environment variables, required tools, file existence) and logs failures.

--- 

### Setter‑specific checks


| Setter | Checks performed |
| :--- | :--- |
| **GnomeSetter** | `XDG_CURRENT_DESKTOP` contains GNOME; file exists; GIO initialised |
| **KdeSetter** | None explicit (assumes Plasma shell is running) |
| **HyprlandSetter** | `HYPRLAND_INSTANCE_SIGNATURE` set; `which hyprpaper` or `which swww` |
| **FallbackSetter** | None (always logs error) |


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
│   └── FallbackSetter.hpp/cpp
```

## 🧪 Tested Distributions (by setter)


| Setter | Tested on |
| :--- | :--- |
| **GnomeSetter** | Fedora 40+, Ubuntu 24.04+, Arch Linux (GNOME) |
| **KdeSetter** | Arch Linux (KDE)|
| **HyprlandSetter** | not tested |

> **Your distribution not listed?** The setter should work as long as the required tools/APIs are present.

---

## 🚧 Limitations & Future Work

*   **No runtime DE switching** – the binary is tied to one DE at compile time.
*   **Multi‑monitor** is handled by the underlying desktop API (KDE script applies to all desktops; GNOME applies to primary monitor; Hyprland depends on the tool used).
*   **Wayland/X11** – GNOME and KDE setters work under both; Hyprland is Wayland‑only.

### Planned improvements:

- Add Windows support.
- Add Cinnamon support.

<div align="center">

# Rwal – one build, one desktop, reliable wallpaper switching. 🐧

[![GitHub Repository](https://shields.io)](https://github.com/Aloncie/Rwal)

[Report an issue](https://github.com) • [Request a setter](https://github.com)

</div>

---

