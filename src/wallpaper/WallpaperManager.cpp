#include "WallpaperManager.hpp"
#include "internal/GlobalConstans.hpp"
#include "logs/logs.hpp"
#include "net/NetworkManager.hpp"
#include "PathResolver.hpp"

#include <QStandardPaths>
#include <filesystem>
#include <QCoreApplication>

namespace fs = std::filesystem;

WallpaperManager::WallpaperManager(UIManager& ui, Keywords& keywords, NetworkManager& nm,
                                   IWallpaperSetter& env)
    : m_ui(ui), m_keywords(keywords), m_nm(nm), m_env(env) {}

std::string WallpaperManager::refresh(const std::string mode) {
	std::string returnMessage = "Wallpaper refreshed successfully";
    m_keywords.GetRandomKeywords(
        [&](std::string keyword) {
            std::optional<fs::path> path = m_nm.fetchImage(keyword);
            if (path.has_value()) {
                PathResolver::toHostPath(*path);
                bool success = m_env.setWallpaper(*path);
                if (!success) {
					returnMessage = "Failed to set wallpaper. More info in logs";
					Logs::getInstance().writeLogs("Failed to set wallpaper: " + path->string());
                }
            } else {
                Logs::getInstance().writeLogs("Path is empty");
				returnMessage = "Failed to fetch image. More info in logs";
            } },
        mode);
	return returnMessage;
}

std::string WallpaperManager::saveCurrent() {
    fs::path current = getCurrentWallpaperPath();
    if (current.empty()) {
        Logs::getInstance().writeLogs("saveCurrent: no wallpaper path");
        return "No current wallpaper to save";
    }

    auto picturesPathOpt = getPicturesPath();
    if (picturesPathOpt->empty()) {
        Logs::getInstance().writeLogs("Could not determine Pictures folder");
        return "Could not determine Pictures folder";
    }

    fs::path dest = *picturesPathOpt / current.filename();
    try {
        fs::copy_file(current, dest, fs::copy_options::overwrite_existing);
        Logs::getInstance().writeLogs("Wallpaper saved to " + dest.string());
        return "Wallpaper saved successfully";
    } catch (const std::exception& e) {
        Logs::getInstance().writeLogs("Failed to save wallpaper: " + std::string(e.what()));
        return "Failed to save wallpaper";
    }
    return "";
}

fs::path WallpaperManager::getCurrentWallpaperPath() const {
    fs::path dir =
        fs::path(
            QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString()) /
        rwal::wallpaper::DONWLOADS_DIR_NAME;
    if (!fs::exists(dir)) return "";

    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (fs::is_regular_file(entry.path())) {
                std::string name = entry.path().filename().string();
                if (name.rfind(rwal::wallpaper::FILE_PREFIX, 0) == 0) {
                    return entry.path();
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        Logs::getInstance().writeLogs("Error scanning downloads: " + std::string(e.what()));
    }
    return "";
}

std::optional<fs::path> WallpaperManager::getPicturesPath() {
    Logs::getInstance().writeLogs("Trying to locate Pictures folder");
    QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (path.isEmpty()) {
        m_ui.showMessage("Could not find Pictures folder");
        return std::nullopt;
    }

    fs::path rwalDir =
        fs::path(path.toStdString()) / QCoreApplication::applicationName().toStdString();
    std::error_code ec;
    fs::create_directories(rwalDir, ec);
    if (ec) {
        m_ui.showMessage("Failed to create rwal directory");
        Logs::getInstance().writeLogs("Failed to create " + rwalDir.string() + ": " + ec.message());
        return std::nullopt;
    }
    return rwalDir;
}

