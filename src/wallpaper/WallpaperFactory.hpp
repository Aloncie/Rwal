#pragma once
#include <memory>

class WallpaperFactory{
public:
	std::unique_ptr<IWallpaperSetter> create();
};
