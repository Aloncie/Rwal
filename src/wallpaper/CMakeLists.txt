# These files are ALWAYS needed

set(SETTERS_SOURCES 
	IWallpaperSetter.hpp 
	WallpaperManager.cpp 
	WallpaperFactory.cpp 
	PathResolver.cpp
)

# These files are ONLY added if the OS matches
if(WIN32)
	list(APPEND SETTERS_SOURCES platforms/WindowsSetter.cpp)
if (UNIX AND NOT APPLE)
	find_package(Qt5Dbus REQUIRED)
	list(APPEND SETTERS_SOURCES platforms/KdeSetter.cpp)
endif()

# The compiler ONLY compiles the files in this list

add_library(wallpaper ${SETTERS_SOURCES})
