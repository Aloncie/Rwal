#include "FileSystemFactory.hpp"

// NativeFileSystem already included in OS-specific headers
#ifdef __linux__
	#include "LinuxFileSystem.hpp"
#elif defined _WIN32
	#include "WindowsFileSystem.hpp"
#endif

std::unique_ptr<IFileSystem> createPlatformFileSystem() {
#ifdef __linux__
	return std::make_unique<LinuxFileSystem>();
#elif defined _WIN32
	return std::make_unique<WindowsFileSystem>();
#else
	#error "Unsupported platform"
#endif
}

