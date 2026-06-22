#pragma once
#include "IWallpaperSetter.hpp"
#include "logs/Logs.hpp"

#include <memory>

std::unique_ptr<IWallpaperSetter> createWallpaperSetter(Logs& logs);
