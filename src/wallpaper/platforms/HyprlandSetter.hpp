#pragma once
#include "wallpaper/IWallpaperSetter.hpp"

#include <string>
#include <filesystem>

class HyprlandSetter : public IWallpaperSetter {
public:
    bool setWallpaper(const fs::path& path) override;
    ~HyprlandSetter() override = default;
};

