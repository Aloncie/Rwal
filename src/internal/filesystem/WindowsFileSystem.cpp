#include "WindowsFileSystem.hpp"

#include <cstdlib>

// Question: Why fallback value instead of std::optional?
//
// The contract is: "I will always give you a usable directory path." 
// current_path() guarantees that. No optional, no error propagation, no caller burden.

fs::path WindowsFileSystem::getAppLocalDataLocation() const {
	if (const char* localappdata = std::getenv("LOCALAPPDATA")){
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
    if (const char* temp = std::getenv("TMP")) {
    	return fs::path(temp);
    }
    // Fallback value
    return fs::current_path() / "tmp";
}
