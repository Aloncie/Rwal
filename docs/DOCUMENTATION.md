# 📖 Documentation

## 💨Quick Start

You must be in Rwal/build to use these commands:
```bash
# interactive start
./rwal

# change mode
./rwa -c
```

For use Rwal everywhere use ```sudo make install```( you must be in Rwal/build ). It add program to /bin and you will can use:
```bash
# everywhere 
rwal
# or change mode
rwal -c
```

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
| `-c` | `--change` | Change wallpaper one and exit |
| '-s' | '--save' | Save current wallpaper |
| '-v' | '--version' | Displays version information |
| '-h' | '--help' | Displays version information |
| none | '--contact' | Displays contact methods |


### Terminal UI Controls

*   **1-q**: Select action( no confirm )

## 🖥️ Environment Support

Rwal uses different backends depending on your Desktop Environment:

*   **KDE Plasma**: D-Bus `org.kde.plasmashell`
*   **GNOME**: `gsettings` (requires glib2)
*   **Hyprland**: 'hyprpaper' or 'hyprctl'

## 🛠️ Known issues

*   **Keywords functional**: Partially broken - input of user in adding/removing( keywords menu ) doesn't show
*   **Internal Timer**: Broken. It is recommended to use a **systemd.timer** or **cron** for intervals for now.
