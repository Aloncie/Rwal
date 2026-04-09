#include "FallbackSetter.hpp"
#include "logs/logs.hpp"
#include "ui/tui/TUIManager.hpp"

bool FallbackSetter::setWallpaper(const fs::path& path) {
	m_logs.writeLogs("No wallpaper setter available for this environment.");
	return false;
}

