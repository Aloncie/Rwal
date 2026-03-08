#pragma once
#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace rwal::platform::executor{
	void open_editor(fs::path& path);
}
