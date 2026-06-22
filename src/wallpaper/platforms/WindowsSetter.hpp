#pragma once
#include "logs/Logs.hpp"
#include "wallpaper/IWallpaperSetter.hpp"

#include <filesystem>

namespace fs = std::filesystem;

class WindowsSetter : public IWallpaperSetter {
private:
    Logs& m_logs;

public:
    bool setWallpaper(const fs::path& path) override;
    bool isAvailable() const override { return true; }
    explicit WindowsSetter(Logs& logs) : m_logs(logs) {}
    ~WindowsSetter() override = default;
};
