#include "LinuxFileSystem.hpp"

#include <cstdlib>
#include <unistd.h>
#include <limits.h>

// Question: Why fallback value instead of std::optional?
//
// The contract is: "I will always give you a usable path." 
// current_path() guarantees that. No optional, no error propagation, no caller burden.

fs::path LinuxFileSystem::getAppLocalDataLocation() const {
    if (const char* xdg = std::getenv("XDG_DATA_HOME")) {
        return fs::path(xdg);
	}
    if (const char* home = std::getenv("HOME")) {
        return fs::path(home) / ".local" / "share";
	}
	// Fallback value
    return fs::current_path() / ".local" / "share";
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

fs::path LinuxFileSystem::getConfigLocation() const {
    if (const char* home = std::getenv("HOME")) {
        return fs::path(home) / ".config";
    }
    // Fallback value
    return fs::current_path() / ".config";
}

fs::path LinuxFileSystem::getScheduleLocation() const {
    if (const char* home = std::getenv("HOME")) {
        return fs::path(home) / ".config/systemd/user";
    }
    // Fallback value
    return fs::current_path() / ".config/systemd/user";
}

fs::path LinuxFileSystem::getBinaryLocation() const {
	char buffer[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer));
	if (len != -1) {
		buffer[len] = '\0';
        return fs::path(buffer);
    }
    // Fallback value
	return fs::current_path() / "rwal";
}

