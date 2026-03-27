#include "HyprlandSetter.hpp"
#include <cstdlib>
#include "logs/logs.hpp"

static bool hasTool(const std::string& tool) {
    std::string cmd = "which " + tool + " > /dev/null 2>&1";
    return (std::system(cmd.c_str()) == 0);
}

bool HyprlandSetter::setWallpaper(const fs::path& path) {
    std::string pathStr = path.string();

    if (hasTool("hyprpaper")) {
        std::system(("hyprctl hyprpaper preload " + pathStr).c_str());
        std::system(("hyprctl hyprpaper wallpaper \", " + pathStr + "\"").c_str());
        std::system("hyprctl hyprpaper unload all");
        Logs::getInstance().writeLogs("Wallpaper set via hyprpaper: " + pathStr);
        return true;
    } else if (hasTool("swww")) {
        std::system(("swww img " + pathStr).c_str());
        Logs::getInstance().writeLogs("Wallpaper set via swww: " + pathStr);
        return true;
    } else {
        Logs::getInstance().writeLogs("Neither hyprpaper nor swww found. Cannot set wallpaper.");
        return false;
    }
}
