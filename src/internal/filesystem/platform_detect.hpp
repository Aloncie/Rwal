#pragma once

#if defined(_WIN32) || defined(_WIN64)
	#define RWAL_PLATFORM_WINDOWS 1
#elif defined(__linux__)
	#define RWAL_PLATFORM_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
	#define RWAL_PLATFORM_MACOS 1
#else
	#error "Unsupported platform"
#endif

