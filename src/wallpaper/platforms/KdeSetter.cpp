#include "KdeSetter.hpp"

#include <cstdlib>
#include <string>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

bool KdeSetter::setWallpaper(const fs::path& path) {
    std::string cmd = 
        "dbus-send --session --dest=org.kde.plasmashell --type=method_call "
        "/PlasmaShell org.kde.PlasmaShell.evaluateScript "
        "\"string:var allDesktops = desktops(); "
        "for (i=0;i<allDesktops.length;i++) { "
        "d = allDesktops[i]; "
        "d.wallpaperPlugin = 'org.kde.image'; "
        "d.currentConfigGroup = Array('Wallpaper', 'org.kde.image', 'General'); "
        "d.writeConfig('Image', 'file://" + path.string() + "') }\"";

    bool status = std::system(cmd.c_str());
	if (status != 0) {
        m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Failed to set wallpaper");
		return false;
    }
	m_logs.writeLogs(lvl::Info, mod::Wallpaper, "Wallpaper set successfully: " + path.string());
	return true;
}

