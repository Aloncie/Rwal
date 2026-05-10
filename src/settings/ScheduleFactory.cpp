#include "ScheduleFactory.hpp"

#if defined(__linux__)
    #include "LinuxSystemSchedule.hpp"
#elif defined(_WIN32)
	#include "WindowsSystemSchedule.hpp"
#else
	#include "NullSystemSchedule.hpp"
#endif

std::unique_ptr<ISystemSchedule> createPlatformSchedule(Logs& logs) {
#if defined(__linux__)
    return std::make_unique<LinuxSystemSchedule>(logs);
#elif defined(_WIN32)
	return std::make_unique<WindowsSystemSchedule>(logs);
#else
	return std::make_unique<NullSystemSchedule>(logs);
#endif
}

