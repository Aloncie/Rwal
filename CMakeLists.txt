if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
endif()

cmake_minimum_required(VERSION 3.10)

project(rwal VERSION 1.0 LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Widgets)
find_package(Curses REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")

add_executable(rwal ${SOURCES} ${HEADERS})

target_compile_definitions(rwal PRIVATE
    SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_include_directories(rwal PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${CURL_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS}
	${CURSES_INCLUDE_DIRS}
)

target_link_libraries(rwal PRIVATE 
	CURL::libcurl 
	nlohmann_json::nlohmann_json 
	Qt5::DBus
	Qt5::Core
	Qt5::Widgets
	${CURSES_LIBRARIES}	
)


install(TARGETS rwal
	RUNTIME DESTINATION bin
)

