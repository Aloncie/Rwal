#include "WallpaperFactory.hpp"
#include "platforms/FallbackSetter.hpp"
#include "IWallpaperSetter.hpp"

#ifdef _WIN32
    #include "platforms/WindowsSetter.hpp"
#else
    #include "platforms/KdeSetter.hpp"
    #ifdef GIO_FOUND
        #include "platforms/GnomeSetter.hpp"
    #endif
    #include "platforms/HyprlandSetter.hpp"
#endif

#include <memory>
#include <string>
#include <cstdlib>

std::unique_ptr<IWallpaperSetter> createWallpaperSetter(Logs& logs) {
#ifdef _WIN32
    return std::make_unique<WindowsSetter>(logs);
#else
    const char* xdg = std::getenv("XDG_CURRENT_DESKTOP");
    std::string desktop = xdg ? xdg : "";

    if (desktop.find("GNOME") != std::string::npos) {
        #ifdef GIO_FOUND
        return std::make_unique<GnomeSetter>(logs);
        #else
        return std::make_unique<FallbackSetter>(logs);
        #endif
    }
    else if (desktop.find("KDE") != std::string::npos) {
        return std::make_unique<KdeSetter>(logs);
    }
    else if (std::getenv("HYPRLAND_INSTANCE_SIGNATURE")) {
        return std::make_unique<HyprlandSetter>(logs);
    }
    else {
        return std::make_unique<FallbackSetter>(logs);
    }
#endif
}

