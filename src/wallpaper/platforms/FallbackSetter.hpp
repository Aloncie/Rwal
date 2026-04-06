#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include "logs/logs.hpp"

namespace fs = std::filesystem;

class FallbackSetter : public IWallpaperSetter {
private:
	Logs& m_logs;
public:
    bool setWallpaper(const fs::path& path) override;
	~FallbackSetter() = default;
};

