#pragma once
#include <string>

#include "wallpaper/IWallpaperSetter.hpp"

class HyprlandSetter : public IWallpaperSetter {
public:
    bool setWallpaper(const std::string& path) override;
    bool hasTool(const std::string& tool);
    ~HyprlandSetter() override;
};
