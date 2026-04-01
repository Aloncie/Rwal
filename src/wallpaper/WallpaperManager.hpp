#pragma once
#include <string>
#include "ui/cli/UIManager.hpp"
#include "keywords/keywords.hpp"
#include <filesystem>
#include <optional>
#include "net/NetworkManager.hpp"
#include "IWallpaperSetter.hpp"

namespace fs = std::filesystem;

class WallpaperManager {
public:
    WallpaperManager(UIManager& ui, Keywords& keywords, NetworkManager& nm, IWallpaperSetter& env);
    std::string refresh(const std::string mode = "core");
    std::string saveCurrent();
    fs::path getCurrentWallpaperPath() const;
    std::optional<fs::path> getPicturesPath();

private:
    UIManager& m_ui;
    Keywords& m_keywords;
    NetworkManager& m_nm;
    IWallpaperSetter& m_env;
};
