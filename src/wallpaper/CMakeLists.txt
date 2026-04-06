set(SETTERS_SOURCES 
    WallpaperManager.cpp 
    WallpaperFactory.cpp 
    PathResolver.cpp
    platforms/FallbackSetter.cpp

	${PROJECT_SOURCE_DIR}/src/net/NetworkManager.cpp
    ${PROJECT_SOURCE_DIR}/src/net/CurlWrapper.cpp
)

# Add an option to force a specific desktop environment for testing or if auto-detection fails
set(RWAL_FORCE_DE "" CACHE STRING "Force a specific desktop environment (GNOME, KDE, Hyprland)")

# Check if the user has set a forced DE and validate it
if(NOT RWAL_FORCE_DE STREQUAL "")
    string(TOUPPER "${RWAL_FORCE_DE}" FORCE_DE_UPPER)
    
    if(FORCE_DE_UPPER MATCHES "GNOME|KDE|HYPRLAND")
        set(CURRENT_DE ${FORCE_DE_UPPER})
        message(STATUS "Using forced desktop environment: ${CURRENT_DE}")
    else()
        message(WARNING "Forced DE '${RWAL_FORCE_DE}' is invalid. Switching to auto-detect.")
        set(RWAL_FORCE_DE "" CACHE STRING "" FORCE)
    endif()
endif()

# If no forced DE, try to auto-detect
if(RWAL_FORCE_DE STREQUAL "")
    set(CURRENT_DE "$ENV{XDG_CURRENT_DESKTOP}")
    string(TOUPPER "${CURRENT_DE}" CURRENT_DE)
    message(STATUS "Auto-detected desktop environment: ${CURRENT_DE}")
endif()

# Add platform-specific sources and definitions based on the detected desktop environment
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
        
    elseif(CURRENT_DE MATCHES "HYPRLAND")
        list(APPEND SETTERS_SOURCES platforms/HyprlandSetter.cpp)
        add_definitions(-DRWAL_USE_HYPRLAND)
        message(STATUS "Using Hyprland wallpaper setter")
        
    else()
        message(STATUS "No supported DE detected ('${CURRENT_DE}'). Using fallback setter.")
        message(STATUS "  (To force a specific DE, use: cmake .. -DRWAL_FORCE_DE=GNOME|KDE|Hyprland)")
    endif()
endif()

# Add the library target for the wallpaper setters
add_library(wallpaper_lib STATIC ${SETTERS_SOURCES})

target_compile_definitions(wallpaper_lib PRIVATE QT_NO_KEYWORDS)

target_include_directories(wallpaper_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

# Platform-specific libraries if needed
if(CURRENT_DE MATCHES "GNOME")
    target_include_directories(wallpaper_lib PRIVATE ${GIO_INCLUDE_DIRS})
    target_link_libraries(wallpaper_lib PRIVATE ${GIO_LIBRARIES})
endif()

if(CURRENT_DE MATCHES "KDE")
    target_link_libraries(wallpaper_lib PRIVATE Qt5::DBus)
endif()

target_link_libraries(wallpaper_lib PRIVATE Qt5::Core)


