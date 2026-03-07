#pragma once
#include <string>
#include "ui/cli//UIManager.hpp"
#include "keywords/keywords.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class WallpaperManager {
public:
    WallpaperManager(UIManager& ui, Keywords& keywords);
    void refresh(const std::string& mode = "core");
    void saveCurrent();
    fs::path getCurrentWallpaperPath() const;
    fs::path getPicturesPath();
private:
    UIManager& m_ui;
    Keywords& m_keywords;
};
