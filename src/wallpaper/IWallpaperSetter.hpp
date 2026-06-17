#pragma once
#include <filesystem>
namespace fs = std::filesystem;

class IWallpaperSetter {
public:
    IWallpaperSetter() = default;
    virtual bool setWallpaper(const fs::path& path) = 0;
    virtual ~IWallpaperSetter() = default;
};
