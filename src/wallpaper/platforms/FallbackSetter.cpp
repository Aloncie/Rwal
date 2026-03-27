#include "FallbackSetter.hpp"
#include "logs/logs.hpp"

void FallbackSetter::setWallpaper(const fs::path& path) {
	Logs::getInstance().writeLogs("No wallpaper setter available for this environment.");
}
