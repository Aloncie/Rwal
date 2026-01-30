#pragma once
#include <string>


namespace rwal::ui{
	struct CharacterMenuConfig;
	struct ArrowMenuConfig;

	namespace io{
		std::string CharacterInput(const CharacterMenuConfig& config);
		std::string ArrowInput(const ArrowMenuConfig& config);
	}
}
