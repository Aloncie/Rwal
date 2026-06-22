#pragma once
#include "ISystemScheduler.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "logs/Logs.hpp"

#include <memory>

std::unique_ptr<ISystemScheduler> createPlatformScheduler(Logs& logs, IFileSystem& fs);
