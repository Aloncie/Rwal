#pragma once

#include <string_view>

namespace rwal::logs::type{
	inline constexpr std::string_view Debug = "[DEBUG]";
	inline constexpr std::string_view Info = "[INFO]";
	inline constexpr std::string_view Warning = "[WARNING]";
	inline constexpr std::string_view Error = "[ERROR]";
	inline constexpr std::string_view Fatal = "[FATAL]";
}
