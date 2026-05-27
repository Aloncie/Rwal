#include "LinuxFileSystem.hpp"

#include <cstdlib>

// Question: Why fallback value instead of std::optional?
//
// The contract is: "I will always give you a usable directory path." 
// current_path() guarantees that. No optional, no error propagation, no caller burden.

fs::path LinuxFileSystem::getAppLocalDataLocation() const {
    if (const char* xdg = std::getenv("XDG_DATA_HOME")) {
        return fs::path(xdg);
	}
    if (const char* home = std::getenv("HOME")) {
        return fs::path(home) / ".local" / "share";
	}
	// Fallback value
    return fs::current_path();
}

fs::path LinuxFileSystem::getPicturesLocation() const {
    if (const char* home = std::getenv("HOME")) {
        return fs::path(home) / "Pictures";
	}
	// Fallback value
    return fs::current_path() / "Pictures";
}

fs::path LinuxFileSystem::getTempLocation() const {
	if (const char* tmp = std::getenv("TMPDIR")) {
		return fs::path(tmp);
	}
	// Fallback value
    return fs::current_path() / "tmp";
}

