#include "WindowsSetter.hpp"

#include <windows.h>
#include <string>

bool setWallpaper(const fs::path& path) {
	std::string widePath = path.wstring();

	BOOL result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, widePath.c_str(), SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
	if (!result) {
		DWORD error = GetLastError();
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "Failed to set wallpaper: " + std::to_string(error));
		return false;
    }
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Wallpaper, "Wallpaper set successfully: " + path.string());
    return true;
}

