#pragma once
#include "logs/logs.hpp"
#include "wallpaper/IWallpaperSetter.hpp"

#include <filesystem>
#include <string>

class HyprlandSetter : public IWallpaperSetter {
private:
    Logs& m_logs;

public:
    HyprlandSetter(Logs& logs);
    bool setWallpaper(const fs::path& path) override;
    ~HyprlandSetter() override = default;
};
