#include "FallbackSetter.hpp"
#include "logs/logs.hpp"
#include "ui/tui/TUIManager.hpp"

bool FallbackSetter::setWallpaper(const fs::path& path) {
	m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "This platform does not support setting wallpapers");
	return false;
}

