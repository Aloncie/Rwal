set(SETTERS_SOURCES 
    WallpaperManager.cpp 
    WallpaperFactory.cpp 
    PathResolver.cpp
)

set(CURRENT_DE "$ENV{XDG_CURRENT_DESKTOP}")

if(WIN32)
    list(APPEND SETTERS_SOURCES platforms/WindowsSetter.cpp)

elseif(UNIX AND NOT APPLE)
    if(CURRENT_DE MATCHES "GNOME")
        message(STATUS "Build: GNOME Support Enabled")

		find_package(PkgConfig REQUIRED)
		pkg_check_modules(GIO REQUIRED gio-2.0)

        list(APPEND SETTERS_SOURCES platforms/GnomeSetter.cpp)
		add_definitions(-DRWAL_USE_GNOME)

		target_include_directories(wallpaper_lib PRIVATE ${GIO_INCLUDE_DIRS})
        target_link_libraries(wallpaper_lib PRIVATE ${GIO_LIBRARIES})

    elseif(CURRENT_DE MATCHES "KDE")
        message(STATUS "Build: KDE Support Enabled")
        find_package(Qt5 COMPONENTS DBus REQUIRED) 
        list(APPEND SETTERS_SOURCES platforms/KdeSetter.cpp)
		add_definitions(-DRWAL_USE_KDE)

    else()
        message(STATUS "Desktop Environment: Other (${CURRENT_DE})")
    endif()
endif()

add_library(wallpaper_lib STATIC ${SETTERS_SOURCES})

target_include_directories(wallpaper_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

# Every platform needs Core
target_link_libraries(wallpaper_lib PRIVATE Qt5::Core)

if(CURRENT_DE MATCHES "KDE")
    target_link_libraries(wallpaper_lib PRIVATE Qt5::DBus)
endif()

target_include_directories(wallpaper_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
