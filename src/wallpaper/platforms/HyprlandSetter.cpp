#include "HyprlandSetter.hpp" 
#include "logs/logs.hpp"

#include <cstdlib>

static bool hasTool(const std::string& tool) {
    std::string cmd = "which " + tool + " > /dev/null 2>&1";
    return (std::system(cmd.c_str()) == 0);
}

bool HyprlandSetter::setWallpaper(const fs::path& path) {
    std::string pathStr = path.string();

	// Check if we're running inside Hyprland by looking for the environment variable it sets.
    if (std::getenv("HYPRLAND_INSTANCE_SIGNATURE") == nullptr) {
        Logs::getInstance().writeLogs("Skipping Hyprland: environment variable HYPRLAND_INSTANCE_SIGNATURE not found.");
        return false;
    }

    if (hasTool("hyprpaper")) {
		// Preload the image with hyprpaper before setting it as wallpaper. This is necessary to ensure the image is available when we set it.
        if (std::system(("hyprctl hyprpaper preload " + pathStr).c_str()) != 0) {
            Logs::getInstance().writeLogs("Failed to preload wallpaper: " + pathStr);
            return false;
        }
       
		// The command to set the wallpaper with hyprpaper is a bit unusual, it requires a comma before the path.
        if (std::system(("hyprctl hyprpaper wallpaper \" , " + pathStr + "\"").c_str()) != 0) {
            Logs::getInstance().writeLogs("Failed to set wallpaper: " + pathStr);
            return false;
        }

		// After setting the wallpaper, we need to reload Hyprland to apply the changes. This is a quirk of how hyprpaper works.
		if (std::system("hyprctl reload") != 0) {
			Logs::getInstance().writeLogs("Failed to reload Hyprland after setting wallpaper: " + pathStr);
			return false;
		}

        Logs::getInstance().writeLogs("Wallpaper set via hyprpaper: " + pathStr);
        return true;

    } else if (hasTool("swww")) {
		// swww is a simpler tool for setting wallpapers in Hyprland, but it doesn't have the same preloading mechanism as hyprpaper. We just set the wallpaper directly.
        if (std::system(("swww img " + pathStr).c_str()) == 0) {
            Logs::getInstance().writeLogs("Wallpaper set via swww: " + pathStr);
            return true;
        }
        Logs::getInstance().writeLogs("Failed to set wallpaper via swww: " + pathStr);
        return false;
    }

    Logs::getInstance().writeLogs("No Hyprland tools (hyprpaper/swww) found.");
    return false;
}

