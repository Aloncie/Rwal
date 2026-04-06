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
private:
	Logs& m_logs;
public:
    WallpaperManager(Logs& logs) : m_logs(logs) {}
	std::string refresh(IWallpaperSetter& m_env, NetworkManager& m_nm, Keywords& m_keywords, UIManager* m_ui = nullptr, const std::string mode = "core");
    std::string saveCurrent() const;
    fs::path getCurrentWallpaperPath() const;
    std::optional<fs::path> getPicturesPath(UIManager* m_ui = nullptr) const;
};

