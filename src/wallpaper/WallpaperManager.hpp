#pragma once

#include "ui/IUserInterface.hpp"
#include "keywords/keywords.hpp"
#include "net/NetworkManager.hpp"
#include "IWallpaperSetter.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <string>
#include <filesystem>
#include <optional>
#include <memory>
#include <string_view>

namespace fs = std::filesystem;

namespace rwal::wallpaper{
	inline constexpr std::string_view FILE_PREFIX = "wallpaper-";
	inline constexpr std::string_view DEFAULT_EXT = ".jpg";
	inline constexpr int SUFFIX_LENGTH = 6;
	inline constexpr std::string_view DONWLOADS_DIR_NAME = "downloads";

}
class WallpaperManager {
private:
	Logs& m_logs;
	IFileSystem& m_fs;
public:
    WallpaperManager(Logs& logs, IFileSystem& fs) : m_logs(logs), m_fs(fs) {}
	virtual ~WallpaperManager() = default;
	virtual std::optional<std::string> refresh(IWallpaperSetter& env, NetworkManager& nm, Keywords& keywords, IUserInterface* ui = nullptr, std::string_view mode = "core");
    virtual std::string saveCurrent() const;
    virtual fs::path getCurrentWallpaperPath() const;
    virtual std::optional<fs::path> getPicturesPath(IUserInterface* ui = nullptr) const;
};

