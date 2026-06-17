#pragma once
#include "logs/logs.hpp"
#include "wallpaper/IWallpaperSetter.hpp"

#include <filesystem>

namespace fs = std::filesystem;

class KdeSetter : public IWallpaperSetter {
private:
    Logs& m_logs;

public:
    explicit KdeSetter(Logs& logs) : m_logs(logs) {}
    ~KdeSetter() override = default;
    bool setWallpaper(const fs::path& path) override;
};
