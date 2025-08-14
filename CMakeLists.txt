if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
endif()

cmake_minimum_required(VERSION 3.10)

project(rwal VERSION 1.0 LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rwal 
	src/main.cpp 
	src/dbus/PlasmaDBus.cpp
	src/keywords/keywords.cpp
	src/net/CurlWrapper.cpp
	src/wallpaper/WallpaperManager.cpp
	src/CLI/CLI.cpp
	src/logs/logs.cpp
)

target_include_directories(rwal PRIVATE
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(rwal PRIVATE 
	CURL::libcurl 
	nlohmann_json::nlohmann_json 
	Qt5::DBus
	Qt5::Core
)

add_custom_target(run
    COMMAND stdbuf -oL $<TARGET_FILE:rwal>
    DEPENDS rwal
)

install(TARGETS rwal
	RUNTIME DESTINATION bin)
