#include "PathResolver.hpp"

#include <string>

void PathResolver::toHostPath(fs::path& path) {
    const char* host_home = std::getenv("HOST_HOME");
    const char* home = std::getenv("HOME");
	if (host_home == nullptr || home == nullptr) return;

	// Use fs::path to avoid bugs of dividers for the different OS
	fs::path homePath = home;
	fs::path hostHomePath = host_home;
	
	if (homePath == hostHomePath) return;

	std::error_code ec;
	auto relativePath = fs::relative(path, homePath, ec);
	if (!ec && !relativePath.empty() && relativePath.string()[0] != '.') {
		path = hostHomePath / relativePath;
	}
}

