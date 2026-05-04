#include "ScheduleFactory.hpp"

#ifdef __linux__
    #include "LinuxSystemSchedule.hpp"
#ifdef WIN32
	#include "WindowsSystemSchedule.hpp"
#endif

std::unique_ptr<ISystemSchedule> createPlatformSchedule(Logs& logs) {
#ifdef __linux__
    return std::make_unique<LinuxSystemSchedule>(logs);
#ifdef WIN32
	return std::make_unique<WindowsSystemSchedule>(logs);
#else
    return std::nullptr;
#endif
}

