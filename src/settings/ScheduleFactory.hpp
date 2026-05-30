#pragma once
#include "ISystemSchedule.hpp"
#include "logs/logs.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <memory>

std::unique_ptr<ISystemSchedule> createPlatformSchedule(Logs& logs, IFileSystem& fs);
