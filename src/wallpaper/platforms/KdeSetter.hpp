#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include "logs/logs.hpp"

#include <filesystem>

namespace fs = std::filesystem;

class KdeSetter : public IWallpaperSetter {
private:
	Logs& m_logs;
public:
	KdeSetter(Logs& logs) : m_logs(logs) {}
	bool setWallpaper(const fs::path& path) override;
};

