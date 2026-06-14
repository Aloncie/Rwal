set(SETTERS_SOURCES
    WallpaperManager.cpp
    WallpaperFactory.cpp
    PathResolver.cpp
    platforms/FallbackSetter.cpp
    platforms/KdeSetter.cpp
    platforms/HyprlandSetter.cpp
    ${PROJECT_SOURCE_DIR}/src/net/NetworkManager.cpp
    ${PROJECT_SOURCE_DIR}/src/net/CurlWrapper.cpp
)

# GnomeSetter only for Unix where there is gio
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GIO gio-2.0)
    if(GIO_FOUND)
        list(APPEND SETTERS_SOURCES platforms/GnomeSetter.cpp)
        message(STATUS "GNOME setter enabled (GIO found)")
    else()
        message(STATUS "GIO not found – GNOME setter disabled")
    endif()
endif()

# WindowsSetter only for Windows
if(WIN32)
    list(APPEND SETTERS_SOURCES platforms/WindowsSetter.cpp)
endif()

add_library(wallpaper_lib STATIC ${SETTERS_SOURCES})

if(GIO_FOUND)
    target_compile_definitions(wallpaper_lib PRIVATE GIO_FOUND=1)
    target_include_directories(wallpaper_lib PRIVATE ${GIO_INCLUDE_DIRS})
    target_link_libraries(wallpaper_lib PRIVATE ${GIO_LIBRARIES})
endif()

if(WIN32)
    target_link_libraries(wallpaper_lib PRIVATE user32 shell32 ws2_32)
endif()

target_link_libraries(wallpaper_lib PUBLIC CURL::libcurl nlohmann_json::nlohmann_json)
target_include_directories(wallpaper_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/generated
)

