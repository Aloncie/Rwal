#pragma once
#include "wallpaper/IWallpaperSetter.hpp"

#include <gmock/gmock.h>
#include <filesystem>

class MockIWallpaperSetter : public IWallpaperSetter {
public:
	MOCK_METHOD(bool, setWallpaper, (const std::filesystem::path& path), (override));
};

