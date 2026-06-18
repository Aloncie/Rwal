#include "WallpaperFactory.hpp"

#include "IWallpaperSetter.hpp"
#include "platforms/FallbackSetter.hpp"

#ifdef _WIN32
#include "platforms/WindowsSetter.hpp"
#else
#include "platforms/GnomeSetter.hpp"
#include "platforms/HyprlandSetter.hpp"
#include "platforms/KdeSetter.hpp"
#endif

#include <cstdlib>
#include <memory>
#include <string>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

std::unique_ptr<IWallpaperSetter> createWallpaperSetter(Logs& logs) {
#ifdef _WIN32
    return std::make_unique<WindowsSetter>(logs);
#else
    const char* xdg = std::getenv("XDG_CURRENT_DESKTOP");
    std::string desktop = xdg ? xdg : "";

    if (desktop.find("GNOME") != std::string::npos) {
        logs.writeLogs(lvl::Info, mod::Wallpaper, "GnomeSetter was chosen.");
        return std::make_unique<GnomeSetter>(logs);
    } else if (desktop.find("KDE") != std::string::npos) {
        logs.writeLogs(lvl::Info, mod::Wallpaper, "KDESetter was chosen.");
        return std::make_unique<KdeSetter>(logs);
    } else if (std::getenv("HYPRLAND_INSTANCE_SIGNATURE")) {
        logs.writeLogs(lvl::Info, mod::Wallpaper, "HyprlandSetter was chosen.");
        return std::make_unique<HyprlandSetter>(logs);
    } else {
        logs.writeLogs(
            lvl::Info, mod::Wallpaper, "No setter was chosen. FallbackSetter will be used.");
        return std::make_unique<FallbackSetter>(logs);
    }
#endif
}
