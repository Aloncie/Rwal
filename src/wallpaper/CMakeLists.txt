set(SETTERS_SOURCES IWallpaperSetter.hpp)

if(WIN32)
	list(APPEND SETTERS_SOURCES platforms/WindowsSetter.cpp)
if (UNIX AND NOT APPLE)
	find_package(Qt5Dbus REQUIRED)
	list(APPEND SETTERS_SOURCES platforms/KdeSetter.cpp)
endif()

add_library(wallpaper ${SETTERS_SOURCES})
