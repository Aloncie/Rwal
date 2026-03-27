#include "WallpaperFactory.hpp

#include <memory>

#include "IWallpaperSetter.hpp"

#ifdef RWAL_USE_KDE
	#include "platforms/KdeSetter.hpp"
#elif defined(RWAL_USE_GNOME)
	#include "platforms/GnomeSetter.hpp"
#elif defined(RWAL_USE_HYPRLAND)
	#include "platforms/HyprlandSetter.hpp"
#endif

std::unique_ptr<IWallpaperSetter> WallpaperFactory::create() {
#ifdef RWAL_USE_KDE
    return std::make_unique<KdeSetter>();
#elif defined(RWAL_USE_GNOME)
    return std::make_unique<GnomeSetter>();
#elif defined(RWAL_USE_HYPRLAND)
    return std::make_unique<HyprlandSetter>();
#else
    return nullptr;
#endif
}
