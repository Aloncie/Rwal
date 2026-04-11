# ⚙️ Configuration

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

