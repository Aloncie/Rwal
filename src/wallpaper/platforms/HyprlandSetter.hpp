#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include "logs/logs.hpp"

#include <string>
#include <filesystem>

class HyprlandSetter : public IWallpaperSetter {
private:
	Logs& m_logs;
public:
	HyprlandSetter(Logs& logs);
    bool setWallpaper(const fs::path& path) override;
    ~HyprlandSetter() override = default;
};

