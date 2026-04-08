#pragma once
#include "ui/cli/UIManager.hpp"
#include "keywords/keywords.hpp"
#include "net/NetworkManager.hpp"
#include "IWallpaperSetter.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <string>
#include <filesystem>
#include <optional>
#include <memory>

namespace fs = std::filesystem;

class WallpaperManager {
private:
	Logs& m_logs;
	std::unique_ptr<IFileSystem> m_fs;
public:
    WallpaperManager(Logs& logs, std::unique_ptr<IFileSystem> fs) : m_logs(logs), m_fs(std::move(fs)) {}
	virtual ~WallpaperManager() = default;
	virtual std::optional<std::string> refresh(IWallpaperSetter& m_env, NetworkManager& m_nm, Keywords& m_keywords, UIManager* m_ui = nullptr, const std::string mode = "core");
    virtual std::string saveCurrent() const;
    virtual fs::path getCurrentWallpaperPath() const;
    virtual std::optional<fs::path> getPicturesPath(UIManager* m_ui = nullptr) const;
};

