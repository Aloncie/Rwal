#include "WindowsSetter.hpp"

#include <windows.h>
#include <string>

bool setWallpaper(const fs::path& path) {
	std::string widePath = path.wstring();

	BOOL result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, widePath.c_str(), SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
	if (!result) {
		DWORD error = GetLastError();
		m_logs.writeLogs("Failed to set wallpaper: " + std::to_string(error));
		return false;
    }
	m_logs.writeLogs("Wallpaper set successfully: " + path.string());
    return true;
}

