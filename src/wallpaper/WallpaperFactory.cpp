#include "WallpaperFactory.hpp"
#include "platforms/FallbackSetter.hpp"
#include "IWallpaperSetter.hpp"

#include <memory>

#ifdef RWAL_USE_KDE
    #include "platforms/KdeSetter.hpp"
#elif defined(RWAL_USE_GNOME)
    #include "platforms/GnomeSetter.hpp"
#elif defined(RWAL_USE_HYPRLAND)
    #include "platforms/HyprlandSetter.hpp"
#elif defined(RWAL_USE_WINDOWS)
	#include "platforms/WindowsSetter.hpp"
#endif

std::unique_ptr<IWallpaperSetter> createWallpaperSetter(Logs& m_logs) {
#ifdef RWAL_USE_KDE
    return std::make_unique<KdeSetter>(m_logs);
#elif defined(RWAL_USE_GNOME)
    return std::make_unique<GnomeSetter>(m_logs);
#elif defined(RWAL_USE_HYPRLAND)
    return std::make_unique<HyprlandSetter>(m_logs);
#elif defined(RWAL_USE_WINDOWS)
	return std::make_unique<WindowsSetter>(m_logs);
#else
    return std::make_unique<FallbackSetter>(m_logs);
#endif
}

