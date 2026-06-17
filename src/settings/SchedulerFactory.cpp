#include "SchedulerFactory.hpp"

#if defined(__linux__)
#include "LinuxSystemScheduler.hpp"
#elif defined(_WIN32)
#include "WindowsSystemScheduler.hpp"
#else
#include "NullSystemScheduler.hpp"
#endif

std::unique_ptr<ISystemScheduler> createPlatformScheduler(Logs& logs, IFileSystem& fs) {
#if defined(__linux__)
    return std::make_unique<LinuxSystemScheduler>(logs, fs);
#elif defined(_WIN32)
    return std::make_unique<WindowsSystemScheduler>(logs, fs);
#else
    return std::make_unique<NullSystemScheduler>(logs, fs);
#endif
}
