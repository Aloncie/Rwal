#include "WallpaperManager.hpp"

#include "AppConfig.h"
#include "PathResolver.hpp"
#include "internal/AppConstants.hpp"
#include "logs/logs.hpp"
#include "net/NetworkManager.hpp"

#include <filesystem>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

namespace fs = std::filesystem;

std::optional<std::string> WallpaperManager::refresh(
    IWallpaperSetter& env, NetworkManager& netmanager, Keywords& keywords, IUserInterface* ui,
    std::string_view mode) {
    std::string keyword;
    if (mode == "change") {
        keyword = keywords.SilentGetKeyword();
    } else {
        if (ui == nullptr) {
            m_logs.writeLogs(lvl::Warning, mod::Wallpaper, "UI is null, using SilentGetKeyword");
            keyword = keywords.SilentGetKeyword();
        } else {
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
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper,
            "Failed to copy file: " + current.string() + " to " + dest.string());
        return "Failed to save wallpaper: " + m_fs.getLastError();
    }
    return "Wallpaper uccessfully saved";
}

fs::path WallpaperManager::getCurrentWallpaperPath() const {
    fs::path dir = m_fs.getAppLocalDataLocation() / ORGANIZATION_NAME / APP_NAME /
                   rwal::constants::dirs::DOWNLOADS_DIR;
    m_logs.writeLogs(lvl::Info, mod::Wallpaper, "Trying to get wallpaper from: " + dir.string());
    if (!m_fs.exists(dir)) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper, "Downloads directory doesn't exist: " + dir.string());
        return "";
    }
    std::vector<fs::path> foundFiles =
        m_fs.listDirectory(dir, std::string(rwal::constants::wallpaper::FILE_PREFIX));
    std::string error = m_fs.getLastError();
    if (error != "") {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper,
            "Failed to list directory: " + dir.string() + " with error: " + error);
    }

    if (foundFiles.size() == 0) {
        m_logs.writeLogs(lvl::Error, mod::Wallpaper, "No wallpaper found in downloads");
        return "";
    } else if (foundFiles.size() == 1) {
        m_logs.writeLogs(lvl::Info, mod::Wallpaper, "Wallpaper found: " + foundFiles[0].string());
        return foundFiles[0];
    }

    m_logs.writeLogs(
        lvl::Error, mod::Wallpaper,
        "Some wallpapers found in downloads, but CurlWrapper must guarantee only one. The latest "
        "one will be saved.");

    int i = 0;
    auto input = m_fs.getLastModifiedTime(foundFiles[i]);
    while (input == std::nullopt) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper,
            "Failed to get last modified time of " + std::to_string(i) +
                " wallpaper file: " + foundFiles[i].string());
        i++;
        input = m_fs.getLastModifiedTime(foundFiles[i]);
    }

    fs::file_time_type latestModifiedTime = input.value();
    m_logs.writeLogs(
        lvl::Debug, mod::Wallpaper,
        "First file: " + std::to_string(latestModifiedTime.time_since_epoch().count()));
    fs::path latestFile = foundFiles[i];
    for (int k = i + 1; k < foundFiles.size(); k++) {
        input = m_fs.getLastModifiedTime(foundFiles[k]);
        while (input == std::nullopt) {
            m_logs.writeLogs(
                lvl::Error, mod::Wallpaper,
                "Failed to get last modified time of " + std::to_string(k) +
                    " wallpaper file: " + foundFiles[k].string());
            k++;
            input = m_fs.getLastModifiedTime(foundFiles[k]);
        }
        fs::file_time_type modifiedTime = input.value();
        m_logs.writeLogs(
            lvl::Debug, mod::Wallpaper,
            "File " + std::to_string(k) + ": " +
                std::to_string(modifiedTime.time_since_epoch().count()));
        if (modifiedTime < latestModifiedTime) {
            latestModifiedTime = modifiedTime;
            latestFile = foundFiles[k];
        }
    }
    return latestFile;
}
