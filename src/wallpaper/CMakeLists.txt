set(SETTERS_SOURCES 
    WallpaperManager.cpp 
    WallpaperFactory.cpp 
    PathResolver.cpp
    platforms/FallbackSetter.cpp
)

# Option to force a specific desktop environment
set(RWAL_FORCE_DE "" CACHE STRING "Force a specific desktop environment (GNOME, KDE, Hyprland)")

# Determine which DE to use
if(NOT RWAL_FORCE_DE STREQUAL "")
    # User forced a specific DE
    set(CURRENT_DE ${RWAL_FORCE_DE})
    message(STATUS "Using forced desktop environment: ${CURRENT_DE}")
else()
    # Auto-detect from environment
    set(CURRENT_DE "$ENV{XDG_CURRENT_DESKTOP}")
    message(STATUS "Auto-detected desktop environment: ${CURRENT_DE}")
endif()

if(UNIX AND NOT APPLE)
    if(CURRENT_DE MATCHES "GNOME")
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GIO REQUIRED gio-2.0)
        list(APPEND SETTERS_SOURCES platforms/GnomeSetter.cpp)
        add_definitions(-DRWAL_USE_GNOME)
        message(STATUS "Using GNOME wallpaper setter")
        
    elseif(CURRENT_DE MATCHES "KDE")
        find_package(Qt5 COMPONENTS DBus REQUIRED)
        list(APPEND SETTERS_SOURCES platforms/KdeSetter.cpp)
        add_definitions(-DRWAL_USE_KDE)
        message(STATUS "Using KDE wallpaper setter")
        
    elseif(CURRENT_DE MATCHES "Hyprland")
        list(APPEND SETTERS_SOURCES platforms/HyprlandSetter.cpp)
        add_definitions(-DRWAL_USE_HYPRLAND)
        message(STATUS "Using Hyprland wallpaper setter")
        
    else()
        message(STATUS "No supported DE detected. Using fallback setter.")
        message(STATUS "  (To force a specific DE, use: -DRWAL_FORCE_DE=GNOME/KDE/Hyprland)")
    endif()
endif()

add_library(wallpaper_lib STATIC ${SETTERS_SOURCES})

target_include_directories(wallpaper_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

if(CURRENT_DE MATCHES "GNOME")
    target_include_directories(wallpaper_lib PRIVATE ${GIO_INCLUDE_DIRS})
    target_link_libraries(wallpaper_lib PRIVATE ${GIO_LIBRARIES})
endif()

if(CURRENT_DE MATCHES "KDE")
    target_link_libraries(wallpaper_lib PRIVATE Qt5::DBus)
endif()

target_link_libraries(wallpaper_lib PRIVATE Qt5::Core)
