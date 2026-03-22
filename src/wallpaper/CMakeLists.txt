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
	set(CURRENT_DE "$ENV{XDG_CURRENT_DESKTOP}")

	if (CURRENT_DE MATCHES "GNOME")
		list(APPEND SETTERS_SOURCES platforms/GnomeSetter.cpp)
	if (CURRENT_DE MATCHES "KDE")
		find_package(Qt5Dbus REQUIRED)
		list(APPEND SETTERS_SOURCES platforms/KdeSetter.cpp)
	else()
		message(STATUS "Desktop Environment: Other (%{CURRENT_DE})")
	endif()
endif()

# The compiler ONLY compiles the files in this list

add_library(wallpaper ${SETTERS_SOURCES})
