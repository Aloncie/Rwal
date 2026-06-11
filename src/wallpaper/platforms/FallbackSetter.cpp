#include "FallbackSetter.hpp"
#include "logs/logs.hpp"

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

bool FallbackSetter::setWallpaper(const fs::path& path) {
	m_logs.writeLogs(lvl::Error, mod::Wallpaper, "This platform does not support setting wallpapers");
	return false;
}

