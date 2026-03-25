#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class PathResolver {
public:
  static void toHostPath(fs::path &path);
};
