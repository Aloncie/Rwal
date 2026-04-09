cmake_minimum_required(VERSION 3.14)

project(rwal VERSION 0.6.1.2 LANGUAGES CXX)

# --- SECTION: Configuration ---
option(BUILD_TESTING "Build the testing suite" OFF)

if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 
set(CMAKE_AUTOUIC ON)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/src/internal/AppConfig.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/generated/AppConfig.h"
)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Widgets Concurrent)
find_package(Curses REQUIRED)

# --- SECTION: Source Files & Targets ---
add_subdirectory(src/wallpaper)
add_subdirectory(src/internal/filesystem)

set(SOURCES
    src/AppController/AppController.cpp
    src/funcs/funcs.cpp
    src/internal/platform/env_utils.cpp
    src/internal/utils/string_utils.cpp
    src/keywords/keywords.cpp
    src/logs/logs.cpp
    src/navigator/navigator.cpp
    src/settings/config.cpp
    src/settings/settings.cpp
    src/ui/cli/UIManager.cpp
    src/ui/menus/menus.cpp
	src/Application/Application.cpp
	src/internal/filesystem/FileSystemFactory.cpp
)

add_library(rwal_lib STATIC ${SOURCES})

target_include_directories(rwal_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/tests
	${CMAKE_CURRENT_BINARY_DIR}/generated
	${Qt5Concurrent_INCLUDE_DIRS}
)

target_link_libraries(rwal_lib PUBLIC
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Qt5::Core
    Qt5::DBus
    Qt5::Widgets
	Qt5::Concurrent
	${CURL_LIBRARIES}
    ${CURSES_LIBRARIES}
    wallpaper_lib 
	filesystem_lib
)

add_executable(rwal src/main.cpp)
target_link_libraries(rwal PRIVATE rwal_lib wallpaper_lib)

# --- SECTION: Testing (Submodule Version) ---
if(BUILD_TESTING)
    if(EXISTS "${PROJECT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        add_subdirectory(third_party/googletest)
    else()
        message(FATAL_ERROR "GoogleTest submodule not found! Run: git submodule update --init --recursive")
    endif()

    enable_testing()

    # GLOB ONLY actual .cpp test files. 
    file(GLOB_RECURSE TEST_SOURCES 
        "tests/unit/*.cpp"
        "tests/integration/*.cpp"
    )
    
    # Ensure test_main.cpp is the only entry point
    list(REMOVE_ITEM TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/test_main.cpp")
    
    add_executable(rwal_tests
        tests/test_main.cpp
        ${TEST_SOURCES}
    )

    # Force C++20 for tests to ensure GTest 1.14+ features work
    target_compile_features(rwal_tests PRIVATE cxx_std_20)

    # Add Mocks and Fixtures as INCLUDE paths so tests can find them
    target_include_directories(rwal_tests PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/tests/mocks
        ${PROJECT_SOURCE_DIR}/tests/fixtures
    )
    
    target_link_libraries(rwal_tests PRIVATE 
        rwal_lib 
        gtest_main 
        gmock_main
    )

    include(GoogleTest)
    gtest_discover_tests(rwal_tests)
endif()

# --- SECTION: Installation & Packaging ---
install(TARGETS rwal RUNTIME DESTINATION bin)

set(CPACK_PACKAGE_NAME "rwal")
set(CPACK_PACKAGE_VENDOR "Aloncie")
set(CPACK_PACKAGE_CONTACT "Aloncie@proton.me")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-alpha")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Wallpaper manager for KDE, GNOME, and Hyprland")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

# Debian/Ubuntu specific
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Aloncie <Aloncie@proton.me>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libcurl4, libstdc++6, libqt5core5a, libqt5widgets5, libqt5dbus5, libncurses6")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_GENERATOR "DEB;RPM;TGZ")
include(CPack)
