# ⚙️ Configuration

Rwal reads its settings from `~/.config/Aloncie/Rwal/config.json`.  
The file is **created automatically** on first run with sensible defaults.

> 💡 Changes to the config file are detected automatically — no restart required.

## 📁 Minimal Configuration

If you just want to set keywords, this is all you need:

```json
{
    "search": {
        "keywords": ["nature", "space", "abstract"]
    }
}
```

All other fields will use their default values.

---

## 📋 Full Configuration Reference

### `search` — Wallpaper Search Settings

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `keywords` | `string[]` | `["nature", "landscape", "abstract", "space", "architecture", "animals", "anime", "cars"]` | Tags used to find wallpapers. A random keyword is selected each refresh. |
| `res` | `string` | `"1920x1080"` | Desired resolution. Format: `WIDTHxHEIGHT`. Use commas for multiple: `"1920x1080,2560x1440"`. |
| `sorting` | `string` | `"random"` | How results are ordered. Valid values: `"random"`, `"toplist"`, `"date_added"`, `"views"`, `"favorites"`. |
| `random_page` | `boolean` | `true` | If `true`, picks a random page of results for more variety. |

### `services.wallhaven` — Wallhaven API Integration

> ⚠️ **API Key Required for Frequent Use**  
> Wallhaven allows limited anonymous requests. If you refresh often (~45 per minute), [get a free API key](https://wallhaven.cc/settings/account).

| Field | Type | Description |
|-------|------|-------------|
| `apikey` | `string` | Your Wallhaven API key. Leave empty for anonymous access. |
| `base_url` | `string` | API endpoint. Usually no need to change. |
| `param_names` | `object` | Mapping of field names in the JSON response to field names in the URL. |

### `settings` — Application Behavior

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `cursor-visibility` | `boolean` | `true` | Show or hide the text cursor in the TUI. |

---

## 📝 Example: Full Configuration

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
                "query": "q",
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

---

## ❓ FAQ

**Q: Do I need an API key?**  
A: Not for occasional use. Wallhaven allows ~45 requests per minute anonymously. If you use systemd timers or refresh very often, add an API key.

**Q: Where do I get a Wallhaven API key?**  
A: Create a free account at [wallhaven.cc](https://wallhaven.cc/), then go to [Settings → API Token](https://wallhaven.cc/settings/apitoken).

**Q: Can I add support for Unsplash, Pexels or any other service?**  
A: Not yet — currently only Wallhaven is supported. Contributions welcome!

