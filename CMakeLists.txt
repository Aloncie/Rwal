cmake_minimum_required(VERSION 3.10)

project(main VERSION 1.0 LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main 
	src/main.cpp 
	src/dbus/PlasmaDBus.cpp
	src/keywords/keywords.cpp
	src/net/CurlWrapper.cpp
	src/wallpaper/WallpaperManager.cpp
)

target_include_directories(main PRIVATE
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(main PRIVATE 
	CURL::libcurl 
	nlohmann_json::nlohmann_json 
	Qt5::DBus
	Qt5::Core
)

add_custom_target(run
    COMMAND stdbuf -oL $<TARGET_FILE:main>
    DEPENDS main
)
