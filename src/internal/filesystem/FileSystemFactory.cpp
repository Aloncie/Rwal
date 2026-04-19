#if defined __linux__ || defined _WIN32
	#include "NativeFileSystem.hpp"
#endif

std::unique_ptr<IFileSystem> createPlatformFileSystem() {
#if defined __linux__ || defined _WIN32
	return std::make_unique<NativeFileSystem>();
#else
	#error "Unsupported platform"
#endif
}

