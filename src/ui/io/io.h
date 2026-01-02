#pragma once
#include "ui/cli/cli.h"

namespace rwal::ui::io{
	char characterInput(const characterMenuConfig& config);
	std::string arrowInput(const arrowMenuConfig& config);
}
