#include "FileSystemFactory.hpp"

#ifdef __linux__
	#include "LinuxFileSystem.hpp"
#endif

std::unique_ptr<IFileSystem> createPlatformFileSystem() {
#ifdef __linux__
	return std::make_unique<LinuxFileSystem>();
#endif
}
