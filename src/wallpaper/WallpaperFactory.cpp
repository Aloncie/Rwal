#include "WallpaperFactory.hpp"
#include "IWallpaperSetter.hpp"

#ifdef _WIN32
    #include "platforms/WindowsSetter.hpp"
#elif defined(__linux__)
    #include "platforms/KdeSetter.hpp"
    #include "platforms/GnomeSetter.hpp"
#endif

std::unique_ptr<IWallpaperSetter> WallpaperFactory::create(){

	#ifdef _WIN32
		return std::make_unique<WindowsSetter>();	
	#elif defined(__APPLE__)		
		return std::make_unique<MacosSetter>();
	#else
		std::string de = std::getenv("XDG_CURRENT_DESKTOP");
		if (de == "KDE") return std::make_unique<KdeSetter>();
		else if (de == "GNOME") return std::make_unique<GnomeSetter>();
		// return std::make_unique<LinuxFallbackSetter>();
	#endif
}
