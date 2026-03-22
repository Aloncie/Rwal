#pragma once
#include <memory>
#include "IWallpaperSetter.hpp"

class WallpaperFactory{
public:
	std::unique_ptr<IWallpaperSetter> create();
};
