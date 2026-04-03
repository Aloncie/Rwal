#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class GnomeSetter : public IWallpaperSetter{
public:
	bool setWallpaper(const fs::path& path) override;
	GnomeSetter();
};

