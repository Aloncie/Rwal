#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cctype>

namespace rwal::utils::string {
	inline void format(std::string& str) {
		if (str.empty()) {
			return;
		}
		const char* trim_chars = " \t\n\r\f\v";

		size_t first = str.find_first_not_of(trim_chars);
		if (first == std::string::npos) {
			// string is only trim_chars
			str.clear();
			return;
		}

		// Cut off trailing trim chars
		size_t last = str.find_last_not_of(trim_chars);
		str = str.substr(first, last - first + 1);

		// these symbols can be used in keyword, so we don't need to replace them
		std::string allowed_special = "-+_";
		for (char& c : str) {
			if (!std::isalnum(static_cast<unsigned char>(c)) &&
				allowed_special.find(c) == std::string::npos) {
				c = ' ';
			}
		}

		// Trim AGAIN to remove spaces created in last step
		if (first == std::string::npos) {
			str.clear();
			return;
		}
		last = str.find_last_not_of(' ');
		str = str.substr(first, last - first + 1);
	}
	std::vector<std::string> split_by_space(const std::string& input);
}

