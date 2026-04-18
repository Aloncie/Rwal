#pragma once

#include <string_view>

namespace rwal::logs::module {
	inline constexpr std::string_view Network = "[NETWORK]";
	inline constexpr std::string_view UI = "[UI]";
	inline constexpr std::string_view Wallpaper = "[WALLPAPER]";
	inline constexpr std::string_view Config = "[CONFIG]";
	inline constexpr std::string_view Keywords = "[KEYWORDS]";
	inline constexpr std::string_view Filesystem = "[FILESYSTEM]";
	inline constexpr std::string_view Core = "[CORE]";
	inline constexpr std::string_view Navigator = "[NAVIGATOR]";
}
