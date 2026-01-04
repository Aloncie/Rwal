#pragma once
#include <string>

struct CharacterMenuConfig;
struct ArrowMenuConfig;

namespace rwal::ui::io{

	std::string CharacterInput(const CharacterMenuConfig& config);
	std::string ArrowInput(const ArrowMenuConfig& config);
}
