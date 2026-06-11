#pragma once
#include "wallpaper/IWallpaperSetter.hpp"
#include "logs/logs.hpp"

#include <filesystem>

namespace fs = std::filesystem;

class WindowsSetter : public IWallpaperSetter {
private:
	Logs& m_logs;
public:
    bool setWallpaper(const fs::path& path) override;
	explicit WindowsSetter(Logs& logs) : m_logs(logs) {}
	~WindowsSetter() override = default;
};
