#pragma once

#include "internal/filesystem/IFileSystem.hpp"
#include <memory>

std::unique_ptr<IFileSystem> createPlatformFileSystem();

