#include "WallpaperManager.hpp"
#include "logs/logs.hpp"
#include "net/NetworkManager.hpp"
#include "PathResolver.hpp"
#include "internal/AppConstants.hpp"
#include "AppConfig.h"

#include <filesystem>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

namespace fs = std::filesystem;

std::optional<std::string> WallpaperManager::refresh(IWallpaperSetter& env, NetworkManager& netmanager, Keywords& keywords, IUserInterface* ui, std::string_view mode) {
    std::string keyword;
    if (mode == "change") {
        keyword = keywords.SilentGetKeyword();
    } else {
		if (ui == nullptr) {
			m_logs.writeLogs(lvl::Warning, mod::Wallpaper, "UI is null, using SilentGetKeyword");
			keyword = keywords.SilentGetKeyword();
		}
		else {
			keyword = keywords.InteractiveGetKeyword(*ui);
		}
    }
	
	std::optional<fs::path> path = netmanager.fetchImage(keyword);

	if (!path.has_value()) {
		m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Failed to fetch image");
		return "Failed to fetch image";
	}
	
	PathResolver::toHostPath(*path);
	
	if (!env.setWallpaper(*path)) {
		m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Failed to set wallpaper: " + path->string());
		return "Failed to set wallpaper";
	}
	
	m_logs.writeLogs(lvl::Info, mod::Wallpaper, "Wallpaper set successfully: " + path->string());
	return std::nullopt;
}

std::string WallpaperManager::saveCurrent() const {
    fs::path current = getCurrentWallpaperPath();
    if (!m_fs.exists(current)) {
        return "No current wallpaper file to save";
    }

	fs::path picturesPath = m_fs.getPicturesLocation() / APP_NAME;
    if (!m_fs.exists(picturesPath)) {
        m_logs.writeLogs(lvl::Error, mod::Wallpaper, "No Pictures folder found");
        return "No Pictures folder found";
    }

    fs::path dest = picturesPath / current.filename();
	if (!m_fs.copyFile(current, dest)) {
		m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Failed to copy file: " + current.string() + " to " + dest.string());
		return "Failed to save wallpaper: " + m_fs.getLastError();
	}
	return "Wallpaper saved to: " + dest.string();
}

fs::path WallpaperManager::getCurrentWallpaperPath() const {
    fs::path dir = m_fs.getAppLocalDataLocation() / ORGANIZATION_NAME / APP_NAME / rwal::constants::dirs::DOWNLOADS_DIR;
    if (!fs::exists(dir)){
		m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Downloads directory doesn't exist: " + dir.string());
		return "";
	}
	std::vector<fs::path> foundedFiles;
    try {
		std::vector<fs::path> downloads_files = m_fs.listDirectory(dir);
		for (auto file : downloads_files) {
			std::string name = file.filename().string();
			if (name.rfind(rwal::constants::wallpaper::FILE_PREFIX, 0) == 0) {
				foundedFiles.push_back(file);
			}
		}
    } catch (const fs::filesystem_error& e) {
        m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Error scanning downloads: " + std::string(e.what()));
    }

	if (foundedFiles.size() == 0) {
		m_logs.writeLogs(lvl::Error, mod::Wallpaper, "No wallpaper found in downloads");
		return "";
	} else if (foundedFiles.size() == 1) {
		m_logs.writeLogs(lvl::Info, mod::Wallpaper, "Wallpaper found in downloads: " + foundedFiles[0].string());
		return foundedFiles[0];
	}

	m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Some wallpapers found in downloads, but CurlWrapper must guarantee only one. The latest one will be saved.");
	
	int i = 0;
	auto input = m_fs.getLastModifiedTime(foundedFiles[i]);
	while (input == std::nullopt) {
		m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Failed to get last modified time of " + std::to_string(i) + " wallpaper file: " + foundedFiles[i].string());	
		i++;
		input = m_fs.getLastModifiedTime(foundedFiles[i]);
	}
	
	fs::file_time_type latestModifiedTime = input.value();
	fs::path latestFile = foundedFiles[i];
	for (int k = i + 1; k < foundedFiles.size(); k++){
		input = m_fs.getLastModifiedTime(foundedFiles[k]);
		while (input == std::nullopt) {
			m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Failed to get last modified time of " + std::to_string(k) + " wallpaper file: " + foundedFiles[k].string());
			k++;
			input = m_fs.getLastModifiedTime(foundedFiles[k]);
		}
		fs::file_time_type modifiedTime = input.value();
		if (modifiedTime < latestModifiedTime) {
            latestModifiedTime = modifiedTime;
			latestFile = foundedFiles[k];
        }
    }
	return latestFile;
}

