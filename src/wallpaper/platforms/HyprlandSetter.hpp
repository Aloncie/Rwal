#pragma once
#include "logs/logs.hpp"
#include "wallpaper/IWallpaperSetter.hpp"

class HyprlandSetter : public IWallpaperSetter {
private:
    Logs& m_logs;

public:
    HyprlandSetter(Logs& logs);
    bool setWallpaper(const fs::path& path) override;
    bool isAvailable() const override;
    ~HyprlandSetter() override = default;
};
