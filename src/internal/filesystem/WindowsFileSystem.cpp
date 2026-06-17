#include "WindowsFileSystem.hpp"

#include <cstdlib>
#include <windows.h>

// Question: Why fallback value instead of std::optional?
//
// The contract is: "I will always give you a usable directory path."
// current_path() guarantees that. No optional, no error propagation, no caller burden.

fs::path WindowsFileSystem::getAppLocalDataLocation() const {
    if (const char* localappdata = std::getenv("LOCALAPPDATA")) {
        return fs::path(localappdata);
    }
    // Fallback value
    return fs::current_path() / "AppData";
}

fs::path WindowsFileSystem::getPicturesLocation() const {
    if (const char* userprofile = std::getenv("USERPROFILE")) {
        return fs::path(userprofile) / "Pictures";
    }
    // Fallback value
    return fs::current_path() / "Pictures";
}

fs::path WindowsFileSystem::getTempLocation() const {
    if (const char* tmp = std::getenv("TMP"))
        return fs::path(tmp);
    if (const char* temp = std::getenv("TEMP"))
        return fs::path(temp);

    // Fallback value
    return fs::current_path() / "tmp";
}

fs::path WindowsFileSystem::getConfigLocation() const {
    if (const char* appdata = std::getenv("APPDATA")) {
        return fs::path(appdata);
    }
    // Fallback value
    return fs::current_path() / "AppData";
}

fs::path WindowsFileSystem::getSchedulerLocation() const {
    // Fallback value
    // I don't know how should I correct impelemt this now, but I must give a value.
    return fs::current_path();
}

fs::path WindowsFileSystem::getBinaryLocation() const {
    // Fallback value
    // I don't know how should I correct impelemt this now, but I must give a value.
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return fs::path(buffer);
}
