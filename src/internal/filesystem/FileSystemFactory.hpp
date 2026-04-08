#pragma once

#include "IFileSystem.hpp"
#include <memory>

std::unique_ptr<IFileSystem> createPlatformFileSystem();

