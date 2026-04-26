#pragma once

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

namespace rwal::platform::executor{
	void open_editor(fs::path& path);
}

namespace rwal::systemd{
	int exec(const std::string& command);
}
