#pragma once
#include "ui/cli/UIManager.hpp"
#include "keywords/keywords.hpp"
#include "net/NetworkManager.hpp"
#include "IWallpaperSetter.hpp"

#include <string>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class WallpaperManager {
public:
    WallpaperManager(UIManager& ui, NetworkManager& nm, IWallpaperSetter& env);
	std::string refresh(Keywords& m_keywords, const std::string mode = "core");
    std::string saveCurrent();
    fs::path getCurrentWallpaperPath() const;
    std::optional<fs::path> getPicturesPath();

private:
    UIManager& m_ui;
    NetworkManager& m_nm;
    IWallpaperSetter& m_env;
};

