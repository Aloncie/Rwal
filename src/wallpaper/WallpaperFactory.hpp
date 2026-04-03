#pragma once
#include "IWallpaperSetter.hpp"

#include <memory>

class WallpaperFactory{
public:
	std::unique_ptr<IWallpaperSetter> create();
};

