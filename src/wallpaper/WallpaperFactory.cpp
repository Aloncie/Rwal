#include "WallpaperFactory.hpp"
#include "IWallpaperSetter.hpp"

std::unique_ptr<IWallpaperSetter> WallpaperFactory::create(){
	#ifdef _WIN32
		return std::make_unique<WindowsSetter>();	
	#ifdef defined(__APPLE__)
		return std::make_unique<MacosSetter>();
	#else
		std::string de = std::getenv("XDG_CURRENT_DESKTOP");
		if (de == "KDE") return std::make_unique<KdeSetter>();
		else if (de == "GNOME") return std::make_unique<GnomeSetter>();
		return std::make_unique<LinuxFallbackSetter>();
	#endif
}
