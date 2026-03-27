#pragma once
#include <string>
#include "wallpaper/IWallpaperSetter.hpp"

namespace fs = std::filesystem;

class KdeSetter : public IWallpaperSetter {
public:
	bool setWallpaper(const fs::path& path) override;
};
