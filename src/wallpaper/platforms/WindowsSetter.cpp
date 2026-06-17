#include "WindowsSetter.hpp"

#include <string>
#include <windows.h>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

bool WindowsSetter::setWallpaper(const fs::path& path) {
    std::wstring widePath = path.wstring();

    BOOL result = SystemParametersInfoW(
        SPI_SETDESKWALLPAPER, 0, (PVOID)widePath.data(),
        SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
    if (!result) {
        DWORD error = GetLastError();
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper, "Failed to set wallpaper: " + std::to_string(error));
        return false;
    }
    m_logs.writeLogs(lvl::Info, mod::Wallpaper, "Wallpaper set successfully: " + path.string());
    return true;
}
