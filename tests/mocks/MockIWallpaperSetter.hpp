#pragma once
#include "wallpaper/IWallpaperSetter.hpp"

#include <filesystem>
#include <gmock/gmock.h>

class MockIWallpaperSetter : public IWallpaperSetter {
public:
    MockIWallpaperSetter() = default;
    MOCK_METHOD(bool, setWallpaper, (const std::filesystem::path& path), (override));
};
