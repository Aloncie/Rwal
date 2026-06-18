#pragma once
#include "logs/logs.hpp"
#include "wallpaper/IWallpaperSetter.hpp"

class FallbackSetter : public IWallpaperSetter {
private:
    Logs& m_logs;

public:
    FallbackSetter(Logs& logs) : m_logs(logs) {}
    bool setWallpaper(const fs::path& path) override;
    bool isAvailable() const override { return true; }
    ~FallbackSetter() = default;
};
