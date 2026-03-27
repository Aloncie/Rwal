#pragma once
#include "wallpaper/IWallpaperSetter.hpp"

namespace fs = std::filesystem;

class FallbackSetter : public IWallpaperSetter {
public:
    bool setWallpaper(const fs::path& path) override;
	~FallbackSetter() = default;
};
