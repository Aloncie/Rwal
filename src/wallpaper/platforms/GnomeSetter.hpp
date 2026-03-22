#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class GnomeSetter : public IWallpaperSetter{
public:
	void setWallpaper(const fs::path& path) override;
	GnomeSetter();
	~GnomeSetter() override = default;
};
