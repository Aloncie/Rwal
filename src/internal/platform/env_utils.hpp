#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace rwal::platform::executor{
	void open_editor(fs::path& path);
}
