#include "string_utils.hpp"
#include <sstream>

namespace rwal::utils::string{
	std::vector<std::string> split_by_space(const std::string& input) {
        std::stringstream ss(input);
        std::string segment;
        std::vector<std::string> result;
        
        while (ss >> segment) {
            result.push_back(segment);
        }
        return result;
    }
}
