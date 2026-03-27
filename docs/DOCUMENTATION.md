# 📖 Documentation

## ⚙️ Configuration
Rwal looks for `config.json` in `~/.config/Aloncie/Rwal/`.

**Example `config.json`:**
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
            "4k",
            "city",
            "architecture"
        ],
        "res": "1920x1080",
        "sorting": "random"
    },
    "services": {
        "wallhaven": {
            "apikey": "apikey=" // for VERY often refreshing,
            "base_url": "https://wallhaven.cc/api/v1/search",
            "param_names": {
                "query": "?q=",
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

## 🚀 Usage

### Command Line Interface


| Flag | Long Flag | Description |
| :--- | :--- | :--- |
| `-c` | `--change` | One-shot wallpaper change based on config. |

### Terminal UI Controls

*   **1-q**: Select action( no confirm )

## 🖥️ Environment Support

Rwal uses different backends depending on your Desktop Environment:

*   **KDE Plasma**: D-Bus `org.kde.plasmashell`
*   **GNOME**: `gsettings` (requires glib2)

## 🛠️ Known issues

*   **Keywords functional**: Partially broken - input of user in adding/removing( keywords menu ) doesn't show
*   **Internal Timer**: Broken. It is recommended to use a **systemd.timer** or **cron** for intervals for now.
