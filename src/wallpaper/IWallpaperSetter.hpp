#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class IWallpaperSetter{
public:
	IWallpaperSetter() = default;
	virtual void setWallpaper(const fs::path& path) = 0;	
	virtual ~IWallpaperSetter() = default;
};
