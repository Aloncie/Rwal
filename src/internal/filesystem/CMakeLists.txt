if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(RWAL_PLATFORM_LINUX TRUE)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(RWAL_PLATFORM_WINDOWS TRUE)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(RWAL_PLATFORM_MACOS TRUE)
else()
	message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

if (RWAL_PLATFORM_LINUX)
	target_sources(rwal PRIVATE src/internal/filesystem/LinuxFileSystem.cpp)
endif()

