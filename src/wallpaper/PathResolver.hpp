#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class PathResolver{
public:
	static fs::path toHostPath(const fs::path& path);
};
