#pragma once
#include <string>

class IWallpaperSetter{
public:
	IWallpaperSetter() = default;
	virtual void setWallpaper(const std::string& path) = 0;	
	virtual ~IWallpaperSetter() = default;
};
