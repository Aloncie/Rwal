#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include <string>

class GnomeSetter : public IWallpaperSetter{
public:
	void setWallpaper(const std::string& path) override;
	GnomeSetter();
	~GnomeSetter() override = default;
};
