#pragma once
#include "ISystemScheduler.hpp"
#include "logs/logs.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <memory>

std::unique_ptr<ISystemScheduler> createPlatformScheduler(Logs& logs, IFileSystem& fs);
