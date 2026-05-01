#pragma once
#include "ISystemSchedule.hpp"
#include "logs/logs.hpp"

#include <memory>

std::unique_ptr<ISystemSchedule> createPlatformSchedule(Logs& logs);
