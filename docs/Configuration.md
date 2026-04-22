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
        "random_page": true,
        "res": "1920x1080",
        "sorting": "random"
    },
    "services": {
        "wallhaven": {
            "apikey": ""
            "base_url": "https://wallhaven.cc/api/v1/search",
            "param_names": {
                "query": "q",
                "apikey": "apikey",
                "page": "page",
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

