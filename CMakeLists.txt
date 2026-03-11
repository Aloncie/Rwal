if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
endif()

cmake_minimum_required(VERSION 3.10)

project(rwal VERSION 1.0 LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Widgets)
find_package(Curses REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 
set(CMAKE_AUTOUIC ON)

set(SOURCES
    src/AppController/AppController.cpp
    src/dbus/PlasmaDBus.cpp
    src/funcs/funcs.cpp
    src/internal/platform/env_utils.cpp
    src/internal/utils/string_utils.cpp
    src/keywords/keywords.cpp
    src/logs/logs.cpp
    src/navigator/navigator.cpp
    src/net/CurlWrapper.cpp
    src/net/NetworkManager.cpp
    src/settings/config.cpp
    src/settings/settings.cpp
	src/ui/cli/UIManager.cpp
    src/ui/menus/menus.cpp
    src/wallpaper/WallpaperManager.cpp
)

set(HEADERS
    src/AppController/AppController.hpp
    src/dbus/PlasmaDBus.hpp
    src/funcs/funcs.hpp
    src/internal/platform/env_utils.hpp
    src/internal/utils/string_utils.hpp
    src/internal/utils/vector_utils.hpp
    src/internal/GlobalConstans.hpp
    src/keywords/keywords.hpp
    src/logs/logs.hpp
    src/navigator/navigator.hpp
    src/net/CurlWrapper.hpp
    src/net/NetworkManager.hpp
    src/settings/config.hpp
    src/settings/settings.hpp
	src/ui/cli/UIManager.hpp
    src/ui/menus/menus.hpp
    src/wallpaper/WallpaperManager.hpp
)

add_library(rwal_lib ${SOURCES} ${HEADERS})

target_include_directories(rwal_lib PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(rwal_lib PUBLIC
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Qt5::DBus
    Qt5::Core
    Qt5::Widgets
    ${CURSES_LIBRARIES}
)

add_executable(rwal src/main.cpp)
target_link_libraries(rwal PRIVATE rwal_lib)

install(TARGETS rwal RUNTIME DESTINATION bin)

enable_testing()

add_executable(rwal_tests
    tests/test_main.cpp
    tests/test_string_utils.cpp
)

target_link_libraries(rwal_tests PRIVATE rwal_lib GTest::gtest_main)
target_include_directories(rwal_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)

include(GoogleTest)
gtest_discover_tests(rwal_tests)
