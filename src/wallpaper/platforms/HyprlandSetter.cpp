#include "HyprlandSetter.hpp"

#include <cstdlib>

#include "logs/Logs.hpp"

bool hasTool(const std::string& tool) {
    std::string cmd = "which " + tool + " > /dev/null 2>&1";
    return (std::system(cmd.c_str()) == 0);
}
void HyprlandSetter::setWallpaper(const std::string& path) {
    if (hasTool("hyprpaper")) {
        std::system(("hyprctl hyprpaper preload " + path).c_str());
        std::system(("hyprctl hyprpaper wallpaper \", " + path + "\"").c_str());
        std::system("hyprctl hyprpaper unload all");
    } else if (hasTool("swww")) {
        std::system(("swww img " + path).c_str());
    } else {
        Logs::getInstance().writeLogs(
            "Neither hyprpaper nor swww is running. Cannot set wallpaper.");
    }
}
