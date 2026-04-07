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
	virtual ~WallpaperManager() = default;
	virtual std::optional<std::string> refresh(IWallpaperSetter& m_env, NetworkManager& m_nm, Keywords& m_keywords, UIManager* m_ui = nullptr, const std::string mode = "core");
    virtual std::string saveCurrent() const;
    virtual fs::path getCurrentWallpaperPath() const;
    virtual std::optional<fs::path> getPicturesPath(UIManager* m_ui = nullptr) const;
	virtual std::string CopyFile(const fs::path& current, const fs::path& dest) const;
};

