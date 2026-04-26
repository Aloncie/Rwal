#include "SchedulerFactory.hpp"

#ifdef __linux__
    #include "LinuxSystemScheduler.hpp"
#endif

std::unique_ptr<ISystemScheduler> createPlatformScheduler(Logs& logs) {
#ifdef __linux__
    return std::make_unique<LinuxSystemScheduler>(logs);
#else
    return std::nullptr;
#endif
}
