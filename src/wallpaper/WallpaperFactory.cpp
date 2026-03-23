#include "WallpaperFactory.hpp"
#include "IWallpaperSetter.hpp"
#include <memory>

#ifdef RWAL_USE_KDE
    #include "platforms/KdeSetter.hpp"
#elif defined(RWAL_USE_GNOME)
    #include "platforms/GnomeSetter.hpp"
#elif defined(RWAL_USE_FALLBACK)
    #include "platforms/LinuxFallbackSetter.hpp"
#endif

std::unique_ptr<IWallpaperSetter> WallpaperFactory::create(){
/* soon

	#ifdef _WIN32
		return std::make_unique<WindowsSetter>();	
	#elif defined(__APPLE__)		
		return std::make_unique<MacosSetter>();
*/

	#ifdef RWAL_USE_KDE
        return std::make_unique<KdeSetter>();
    #elif defined(RWAL_USE_GNOME)
        return std::make_unique<GnomeSetter>();
    #elif defined(RWAL_USE_FALLBACK)
        return std::make_unique<LinuxFallbackSetter>();
    #else
        return nullptr; 
    #endif

}
