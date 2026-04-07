#pragma once
#include "ui/cli/UIManager.hpp"
#include "keywords/keywords.hpp"
#include "net/NetworkManager.hpp"
#include "IWallpaperSetter.hpp"

#include <string>
#include <filesystem>
#include <optional>
#include <functional>

namespace fs = std::filesystem;

class WallpaperManager {
private:
	Logs& m_logs;
public:
    WallpaperManager(Logs& logs) : m_logs(logs) {}
	void refresh(IWallpaperSetter& m_env, NetworkManager& m_nm, Keywords& m_keywords, std::function<void(const std::string&)> callback ,UIManager* m_ui = nullptr, const std::string mode = "core");
    std::string saveCurrent() const;
    fs::path getCurrentWallpaperPath() const;
    std::optional<fs::path> getPicturesPath(UIManager* m_ui = nullptr) const;
};

