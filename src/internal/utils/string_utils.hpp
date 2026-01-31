#pragma once
#include <string>
#include <algorithm>
#include <vector>

namespace rwal::utils::str{
	inline void format(std::string& str){
		if (str.empty())
			return;
		const char* whitelist = " \t\n\r\v\f";
		size_t first = str.find_first_not_of(whitelist);
		if (first == std::string::npos){
			str.clear();
			return;
		}
		size_t last = str.find_last_not_of(whitelist);
		str = str.substr(first,(last-first+1));
		
		str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) {
            return !std::isalnum(c) && c != ' ' && c != '-' && c != '+';
        }), str.end());	
		
		first = str.find_first_not_of(whitelist);
        if (first != std::string::npos) {
            last = str.find_last_not_of(whitelist);
            str = str.substr(first, (last - first + 1));
        }
	}
	std::vector<std::string> split_by_space(const std::string& input);
}
