#include "WallpaperManager.hpp"
#include "internal/GlobalConstans.hpp"
#include "logs/logs.hpp"
#include "net/NetworkManager.hpp"
#include "PathResolver.hpp"

#include <QStandardPaths>
#include <filesystem>
#include <QCoreApplication>

namespace fs = std::filesystem;

std::optional<std::string> WallpaperManager::refresh(IWallpaperSetter& env, NetworkManager& nm, Keywords& keywords, IUserInterface* ui, const std::string mode) {
    std::string keyword;
    if (mode == "change") {
        keyword = keywords.SilentGetKeyword();
    } else {
		if (ui == nullptr) {
			m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Wallpaper, "UI is null, using SilentGetKeyword");
			keyword = keywords.SilentGetKeyword();
		}
		else {
			keyword = keywords.InteractiveGetKeyword(*ui);
		}
    }
	
	std::optional<fs::path> path = nm.fetchImage(keyword);

	if (!path.has_value()) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "Failed to fetch image");
		return "Failed to fetch image";
	}
	
	PathResolver::toHostPath(*path);
	
	if (!env.setWallpaper(*path)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "Failed to set wallpaper: " + path->string());
		return "Failed to set wallpaper";
	}
	
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Wallpaper, "Wallpaper set successfully: " + path->string());
	return std::nullopt;
}

std::string WallpaperManager::saveCurrent() const {
    fs::path current = getCurrentWallpaperPath();
    if (!m_fs.exists(current)) {
        m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Wallpaper, "saveCurrent: no wallpaper path");
        return "No current wallpaper file to save";
    }

	fs::path picturesPath = getPicturesPath().value();
    if (picturesPath.empty() || !m_fs.exists(picturesPath)) {
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "No Pictures folder found");
        return "No Pictures folder found";
    }

    fs::path dest = picturesPath / current.filename();
	if (!m_fs.copyFile(current, dest)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "Failed to copy file: " + current.string() + " to " + dest.string());
		return "Failed to save wallpaper: " + m_fs.getLastError();
	}
	return "Wallpaper saved to: " + dest.string();
}

fs::path WallpaperManager::getCurrentWallpaperPath() const {
    fs::path dir = fs::path( QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString()) / rwal::wallpaper::DONWLOADS_DIR_NAME;
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
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "Error scanning downloads: " + std::string(e.what()));
    }
    return "";
}

std::optional<fs::path> WallpaperManager::getPicturesPath(IUserInterface* ui) const {
    m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Wallpaper, "Trying to locate Pictures folder");
    QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (path.isEmpty()) {
		if (ui != nullptr) {
			ui->showMessage("Could not find Pictures folder");
		}
        return std::nullopt;
    }

    fs::path rwalDir = fs::path(path.toStdString()) / QCoreApplication::applicationName().toStdString();
    std::error_code ec;
    fs::create_directories(rwalDir, ec);
    if (ec) {
		if (ui != nullptr) {
			ui->showMessage("Failed to create rwal directory");
		}
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "Failed to create " + rwalDir.string() + ": " + ec.message());
        return std::nullopt;
    }
    return rwalDir;
}

