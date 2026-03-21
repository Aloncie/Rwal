#pragma once
#include <string>
#include "wallpaper/IWallpaperSetter.hpp"

class KdeSetter : public IWallpaperSetter {
public:
	void setWallpaper(const std::string& path) override;
	~KdeSetter() override = 0;
}
