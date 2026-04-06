#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include "logs/logs.hpp"

#include <filesystem>

namespace fs = std::filesystem;

class GnomeSetter : public IWallpaperSetter{
private:
	Logs& m_logs;
public:
	bool setWallpaper(const fs::path& path) override;
	GnomeSetter(Logs& logs);
};

