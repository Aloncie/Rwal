#include "ScheduleFactory.hpp"

#ifdef __linux__
    #include "LinuxSystemSchedule.hpp"
#endif

std::unique_ptr<ISystemSchedule> createPlatformSchedule(Logs& logs) {
#ifdef __linux__
    return std::make_unique<LinuxSystemSchedule>(logs);
#else
    return std::nullptr;
#endif
}
