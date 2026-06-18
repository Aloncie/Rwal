set(SETTERS_SOURCES
    WallpaperManager.cpp
    WallpaperFactory.cpp
    PathResolver.cpp
    platforms/FallbackSetter.cpp
    platforms/KdeSetter.cpp
	platforms/GnomeSetter.cpp
    platforms/HyprlandSetter.cpp
    ${PROJECT_SOURCE_DIR}/src/net/NetworkManager.cpp
    ${PROJECT_SOURCE_DIR}/src/net/CurlWrapper.cpp
)

if(WIN32)
    list(APPEND SETTERS_SOURCES platforms/WindowsSetter.cpp)
endif()

add_library(wallpaper_lib STATIC ${SETTERS_SOURCES})

# dlopen/dlsym for GnomeSetter (dinamicly link gio lib)
if(UNIX AND NOT APPLE)
    target_link_libraries(wallpaper_lib PRIVATE ${CMAKE_DL_LIBS})
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
