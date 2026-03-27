#include "FallbackSetter.hpp"
#include "logs/logs.hpp"
#include "ui/cli/UIManager.hpp"

bool FallbackSetter::setWallpaper(const fs::path& path) {
	Logs::getInstance().writeLogs("No wallpaper setter available for this environment.");
	return false;
}
