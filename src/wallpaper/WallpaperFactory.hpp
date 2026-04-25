#pragma once
#include "IWallpaperSetter.hpp"
#include "logs/logs.hpp"

#include <memory>

std::unique_ptr<IWallpaperSetter> createWallpaperSetter(Logs& logs);

