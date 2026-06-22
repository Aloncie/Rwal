<div align = center>

<img src="https://github.com/Aloncie/Rwal/raw/main/assets/banner.png" width="750" alt="Rwal Banner">

<br>

[![Badge Workflow]][Workflow]
[![Badge Version]][Version]
[![Badge License]][License]
![Badge Language]
![Badge Hi Mom]

<br>

**Rwal** is a lightweight, cross-platform wallpaper manager written in C++20, designed for **speed** and **flexibility**. It bridges the gap between a rich, interactive terminal user interface (TUI) via ncurses and a fast command-line interface (CLI).

---

</div>

# ✨ Features

- **Interactive TUI** – Navigate, tag, and manage wallpapers without leaving the terminal.
- **Command Line Interface** – Customize config, manage wallpaper from the command line with ease.
- **Hot‑Reload** – Configuration updates instantly without restarting the service.
- **Modern C++20** – Built for performance with a clean, modular codebase.
- **Smart Keyword Logic** – Advanced wallpaper selection based on user-defined tags.
- **Scheduler Integration** – Native support for timers and automated rotations.
- **Cross-DE Support** – Works seamlessly for every Desktop Environment from [here](https://github.com/Aloncie/Rwal/blob/main/docs/SupportedDE.md).

# 👁️‍🗨️ Previews

<div align="center">

### Terminal
![Interactive Preview][PreviewTUI]

### Command Line
![Daemon Preview][PreviewCLI]

</div>

# 📦 Installation

## 1. Download the Pre-compiled Binary (Recommend for Users)
This is the easiest method if you just want to use Rwal. Binaries support any Desktop Environment listed [here](https://github.com/Aloncie/Rwal/blob/main/docs/SupportedDE.md).

**Drawbacks:** You won't have the development environment set up, so you won't be able to easily modify the source code or contribute to the project.

### How can you get updates❔
You will need to download the new version manually from the releases page.

👉 [Download the latest release](https://github.com/Aloncie/Rwal/releases/latest)

---

## 2. Build from Source (Recommended for Contributors)
You can install Rwal on any supported OS and distribution using our Python scripts:

```bash
# Clone the repository
git clone https://github.com/Aloncie/Rwal.git
cd Rwal

# Install dependencies and build the binary
python3 setup.py
python3 build.py
```

**Drawbacks:** You need to install development dependencies on your local machine.

### How can you get updates❔
It depends on how you use the repository:

- **As a User:** Simply run `git pull origin main` to fetch the latest changes.
- **As a Contributor:** If you have local changes, use `git fetch origin` followed by `git rebase origin/develop` to keep your git history clean and avoid merge conflicts.

# 🌀 Usage
The build process generates the binary inside the `build/<OS>-release/` directory (where `<OS>` is `linux` or `windows`). Depending on your build configuration, you will get one of three executable flavors:

### 1. Unified Mode (`rwal`)
*Contains both TUI and CLI functionality. This is the default recommended build.*
```bash
# Launch interactive terminal UI
./build/linux-release/rwal

# Run --help option via command line interface
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

**Important:** you can use **only** CLI version on Windows. Rwal doesn't support TUI on Windows. If you want to use TUI — [open the discussion](https://github.com/Aloncie/Rwal/discussions/new/choose).

</details>

# ⚙️ Configuration

By default, **Rwal** looks for `config.json` in the following system directories:
* **Linux:** `~/.config/Aloncie/Rwal/config.json`
* **Windows:** `%APPDATA%\Aloncie\Rwal\config.json`

### Example `config.json`

**Important:** Config will be create for the startup if it doesn't exist. You don't need create it manually.

```json
{
  "search": {
    "keywords": [
      "nature",
      "town",
      "ocean",
      "car",
      "art",
      "city",
      "anime"
    ],
    "random_page": true,
    "res": "1920x1080",
    "sorting": "random"
  },
  "services": {
    "wallhaven": {
      "apikey": "",
      "base_url": "https://wallhaven.cc/api/v1/search",
      "param_names": {
        "apikey": "apikey",
        "page": "page",
        "query": "q",
        "res": "resolutions",
        "sorting": "sorting"
      }
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
| `settings.cursor-visibility`| Boolean| `true` | *(Not available now)* Toggles the visibility of the terminal cursor inside the TUI mode.|


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
[License]: https://github.com/Aloncie/Rwal/blob/main/LICENSE.txt
[Security]: https://github.com/Aloncie/Rwal/blob/main/SECURITY.md
[Roadmap]: https://github.com/Aloncie/Rwal/blob/main/docs/Roadmap.md
[Survey]: https://forms.gle/JCewzzEUwPGxx7cY7

[PreviewTUI]: https://github.com/Aloncie/Rwal/blob/develop/assets/PreviewTUI.gif
[PreviewCLI]: https://github.com/Aloncie/Rwal/blob/develop/assets/PreviewCLI.gif

[libcurl]: https://curl.se/libcurl/
[ncurses]: https://invisible-island.net/ncurses/
[nlohmann/json]: https://github.com/nlohmann/json

[Badge Workflow]: https://github.com/Aloncie/Rwal/actions/workflows/release.yml/badge.svg
[Badge Version]: https://img.shields.io/badge/version-beta-orange?style=flat-square
[Badge License]: https://img.shields.io/badge/license-MIT-gold?style=flat-square
[Badge Platform]: https://img.shields.io/badge/platform-linux%20%20-lightgrey?style=flat-square
[Badge Language]: https://img.shields.io/github/languages/top/Aloncie/Rwal?style=flat-square
[Badge Hi Mom]: https://img.shields.io/badge/Hi-mom!-ff69b4?style=flat-square
