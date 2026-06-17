#pragma once

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

namespace rwal::platform::executor {
void open_editor(const fs::path& path);
}

namespace rwal::systemd {
int exec(const std::string& command);
}
