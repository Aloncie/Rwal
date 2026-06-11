<div align = center>

<img src="https://github.com/Aloncie/Rwal/raw/main/assets/banner.png" width="750" alt="Rwal Banner">

<br>

[![Badge Workflow]][Workflow]
[![Badge Version]][Version]
[![Badge License]][License]
![Badge Language]
![Badge Hi Mom]

<br>

**Rwal** is a lightweight, cross-platform wallpaper utility designed for speed and flexibility. 
It bridges the gap between a rich, interactive **ncurses** interface and a silent **daemon** for automation.

---

</div>

# ✨ Features

- **Interactive TUI** – Navigate, tag, and preview wallpapers without leaving the terminal.
- **Silent Daemon Mode** – Efficient background rotation ideal for cron or systemd.
- **Hot‑Reload** – Configuration updates instantly without restarting the service.
- **Modern C++20** – Built for performance with a clean, modular codebase.
- **Smart Keyword Logic** – Advanced wallpaper selection based on user-defined tags.
- **Scheduler Integration** – Native support for timers and automated rotations.
- **Cross-DE Support** – Works seamlessly for every DE from [here](https://github.com/Aloncie/Rwal/blob/main/docs/SupportedDE.md).

# 👁️‍🗨️ Previews

<div align="center">

### Interactive Mode
![Interactive Preview][PreviewA]

### Change Mode
![Daemon Preview][PreviewB]

</div>

# 💦 Installation
You need use **two** python scripts to install Rwal for any OS & distribution from [here](https://github.com/Aloncie/Rwal/blob/main/docs/SupportedDE.md):

``` bash
git clone https://github.com/Aloncie/Rwal.git       # clone repository

cd Rwal

python setup.py    # download dependencies
python build.py    # build the binary
```

# 🌀 Usage
The build process generates the binary inside the `build/<OS>-release/` directory (where `<OS>` is `linux` or `windows`). Depending on your build configuration, you will get one of three executable flavors:

### 1. Unified Mode (`rwal`)
*Contains both TUI and CLI functionality. This is the default recommended build.*
```bash
# Launch interactive terminal UI
./build/linux-release/rwal

# Run --help option via console interface
./build/linux-release/rwal --help
```

### 2. Standalone Modes (Lightweight)
*If you built specific tailored versions instead of the unified binary:*

* **`rwal-tui`** — Dedicated interactive interface.
  ```bash
  ./build/linux-release/rwal-tui
  ```
* **`rwal-cli`** — Pure command-line tool for automation.
  ```bash
  ./build/linux-release/rwal-cli --change
  ```

---

<details>
<summary>🪟 <b>Windows Users Click Here</b></summary>

On Windows, use the `.exe` extension and your specific shell syntax inside the `windows-preset` folder:
```powershell
# Unified mode example in PowerShell
.\build\windows-release\rwal-cli.exe --help
```

Also, you can use **only** CLI version on Windows.

</details>

# ⚙️ Configuration

By default, **Rwal** looks for `config.json` in the following system directories:
* **Linux:** `~/.config/Aloncie/Rwal/config.json`
* **Windows:** `%APPDATA%\Aloncie\Rwal\config.json`

### Example `config.json`

*Config will be create for the startup if it doesn't exist. You don't need create it manually.*

```json
{
    "search": {
        "keywords": [
            "anime",
            "nature",
            "town",
            "ocean",
            "car",
            "art",
            "city",
            "architecture"
        ],
        "res": "1920x1080",
        "sorting": "random"
    },
    "services": {
        "wallhaven": {
            "apikey": "apikey="
        }
    },
    "settings": {
        "cursor-visibility": "true"
    }
}
```

### 📋 Configuration Parameters

| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `search.keywords` | Array | `[]` | List of tags used to filter and fetch wallpapers. |
| `search.res` | String | `"1920x1080"`| Target image resolution (e.g., `"2560x1440"`). |
| `search.sorting` | String | `"random"` | API sorting method: `"random"`, `"relevance"`, `"views"`, `"date"`, `"toplist"`, `"favorites"`. |
| `services.wallhaven.apikey`| String | `""` | *(Optional)* Your Wallhaven API key to bypass rate limits(~45 per minute) and access NSFW/Sketchy content. |
| `settings.cursor_visibility`| Boolean| `true` | Toggles the visibility of the terminal cursor inside the TUI mode. |


#### ❗WARNING❗:
**API Settings (`base_url` & `param_names`):** These parameters are pre-configured for the official Wallhaven API. **Do not modify them** unless you are setting up a custom API mirror. Changing these values incorrectly will break the wallpaper search and network requests.


<details>
<summary>🛠️ <b>Advanced: Custom API Parameter Mapping</b></summary>

If you want to tweak internal API endpoints or use a custom mirror, the utility fallback structure uses these pre-mapped fields under the hood:
* `base_url`: `https://wallhaven.cc`
* Query mapping handles parameters (`q`, `page`, `apikey`, `resolutions`, `sorting`) automatically based on your search settings.
</details>

# 🛡️ Maintenance

- **Security:** Please see our [Security Policy][Security].
- **Roadmap:** Check our upcoming features [here][Roadmap].
- **Survey:** Help shape Rwal's future via our [Google Form][Survey].

# 🤝 Special Thanks

- **[libcurl]** — For network requests and image downloads.
- **[ncurses]** - For the classic terminal UI capabilities.
- **[nlohmann/json]** - For making C++ JSON handling sane.

<br>

<div align="center">
Made with ❤️ and a lot of ☕ by <b>Aloncie</b>
</div>

[Workflow]: https://github.com/Aloncie/Rwal/actions
[Version]: https://img.shields.io/badge/version-beta-yellow
[License]: https://github.com/Aloncie/Rwal/blob/main/LICENSE
[Security]: https://github.com/Aloncie/Rwal/blob/main/SECURITY.md
[Roadmap]: https://github.com/Aloncie/Rwal/blob/main/docs/ROADMAP.md
[Survey]: https://forms.gle/JCewzzEUwPGxx7cY7

[PreviewA]: https://github.com/user-attachments/assets/9de86ea7-a8f6-4b4b-8098-25ccc14520bb
[PreviewB]: https://github.com/user-attachments/assets/c01c9cea-cbe5-4ddd-8f00-f4731b461fb6

[libcurl]: https://curl.se/libcurl/
[ncurses]: https://invisible-island.net/ncurses/
[nlohmann/json]: https://github.com/nlohmann/json

[Badge Workflow]: https://github.com/Aloncie/Rwal/actions/workflows/release.yml/badge.svg
[Badge Version]: https://img.shields.io/badge/version-beta-orange?style=flat-square
[Badge License]: https://img.shields.io/badge/license-MIT-gold?style=flat-square
[Badge Platform]: https://img.shields.io/badge/platform-linux%20%20-lightgrey?style=flat-square
[Badge Language]: https://img.shields.io/github/languages/top/Aloncie/Rwal?style=flat-square
[Badge Hi Mom]: https://img.shields.io/badge/Hi-mom!-ff69b4?style=flat-square
