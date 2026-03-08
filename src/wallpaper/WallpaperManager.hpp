#pragma once
#include <string>
#include "ui/cli//UIManager.hpp"
#include "keywords/keywords.hpp"
#include <filesystem>
#include <exception>
#include <optional>
#include "net/NetworkManager.hpp"

namespace fs = std::filesystem;

class WallpaperManager {
public:
    WallpaperManager(UIManager& ui, Keywords& keywords, NetworkManager& nm);
    void refresh(const std::string mode = "core");
	std::string saveCurrent();
    fs::path getCurrentWallpaperPath() const;
	std::optional<fs::path> getPicturesPath();
private:
    UIManager& m_ui;
    Keywords& m_keywords;
	NetworkManager& m_nm;
};
