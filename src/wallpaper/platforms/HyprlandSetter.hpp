#pragma once
#include <string>
#include <filesystem>
#include "wallpaper/IWallpaperSetter.hpp"

class HyprlandSetter : public IWallpaperSetter {
public:
    bool setWallpaper(const fs::path& path) override;
    ~HyprlandSetter() override = default;
};
