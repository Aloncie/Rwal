#pragma once
#include <string>
#include "wallpaper/IWallpaperSetter.hpp"

namespace fs = std::filesystem;

class KdeSetter : public IWallpaperSetter {
public:
	void setWallpaper(const fs::path& path) override;
	~KdeSetter() override = 0;
}
