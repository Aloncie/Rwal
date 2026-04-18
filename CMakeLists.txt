cmake_minimum_required(VERSION 3.14)

project(Rwal VERSION 0.6.1.2 LANGUAGES CXX)

# -------------------------------------------------------------------
# Options
# -------------------------------------------------------------------
option(BUILD_TESTING "Build the testing suite" OFF)
option(BUILD_TUI "Build with TUI support (requires ncurses)" ON)
option(BUILD_CLI "Build with CLI support" ON)

# -------------------------------------------------------------------
# Basic Configuration
# -------------------------------------------------------------------
if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 
set(CMAKE_AUTOUIC ON)

# -------------------------------------------------------------------
# Project Metadata
# -------------------------------------------------------------------
string(TIMESTAMP CURRENT_YEAR "%Y")
set(ORGANIZATION_NAME "Aloncie")
set(ORGANIZATION_DOMAIN "io.github.aloncie.rwal")
set(APP_EMAIL "Aloncie@proton.me")
set(APP_COPYRIGHT "Copyright (c) 2025-${CURRENT_YEAR} Aloncie")
set(APP_REPOSITORY_URL "https://github.com/${ORGANIZATION_NAME}/${PROJECT_NAME}")
set(APP_ISSUES_URL "${APP_REPOSITORY_URL}/issues")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/internal/AppConfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/AppConfig.h"
)

# -------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Widgets Concurrent)

# -------------------------------------------------------------------
# Core Library (UI‑agnostic)
# -------------------------------------------------------------------
add_subdirectory(src/wallpaper)
add_subdirectory(src/internal/filesystem)

add_library(rwal-core STATIC
    src/funcs/funcs.cpp
    src/internal/platform/env_utils.cpp
    src/internal/utils/string_utils.cpp
    src/keywords/keywords.cpp
    src/logs/logs.cpp
    src/settings/config.cpp
    src/settings/settings.cpp
    src/internal/filesystem/FileSystemFactory.cpp
)

target_include_directories(rwal-core PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/tests
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${Qt5Concurrent_INCLUDE_DIRS}
)

target_link_libraries(rwal-core PUBLIC
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Qt5::Core
    Qt5::DBus
    Qt5::Widgets
    Qt5::Concurrent
    wallpaper_lib 
    filesystem_lib
)

# -------------------------------------------------------------------
# Executable Targets
# -------------------------------------------------------------------
if (BUILD_TUI AND NOT WIN32 AND BUILD_CLI)
    find_package(Curses REQUIRED)
    add_executable(rwal src/main.cpp src/Application/Application.cpp)
    target_link_libraries(rwal PRIVATE rwal-core ${CURSES_LIBRARIES})
    target_sources(rwal PRIVATE
        src/ui/tui/TUIManager.cpp
        src/AppController/AppController.cpp
        src/navigator/navigator.cpp
        src/ui/tui/menus/menus.cpp
        src/ui/cli/CommandLineParser.cpp
        src/ui/cli/CommandLineHandlers.cpp
    )
    target_compile_definitions(rwal PRIVATE RWAL_USE_TUI=1 RWAL_USE_CLI=1)
    install(TARGETS rwal RUNTIME DESTINATION bin)
    message(STATUS "Building combined rwal (TUI + CLI)")

elseif (BUILD_CLI)
    add_executable(rwal-cli src/ui/cli/main_cli.cpp src/Application/Application.cpp)
    target_link_libraries(rwal-cli PRIVATE rwal-core)
    target_sources(rwal-cli PRIVATE
        src/ui/cli/CommandLineParser.cpp
        src/ui/cli/CommandLineHandlers.cpp
    )
    target_compile_definitions(rwal-cli PRIVATE RWAL_USE_CLI=1)
    install(TARGETS rwal-cli RUNTIME DESTINATION bin)
    message(STATUS "Building CLI-only executable (rwal-cli)")

elseif (BUILD_TUI AND NOT WIN32)
    find_package(Curses REQUIRED)
    add_executable(rwal-tui src/ui/tui/main_tui.cpp src/Application/Application.cpp)
    target_link_libraries(rwal-tui PRIVATE rwal-core ${CURSES_LIBRARIES})
    target_sources(rwal-tui PRIVATE
        src/ui/tui/TUIManager.cpp
        src/AppController/AppController.cpp
        src/navigator/navigator.cpp
        src/ui/tui/menus/menus.cpp
    )
    target_compile_definitions(rwal-tui PRIVATE RWAL_USE_TUI=1)
    install(TARGETS rwal-tui RUNTIME DESTINATION bin)
    message(STATUS "Building TUI-only executable (rwal-tui)")

else()
    message(FATAL_ERROR "No UI selected. Enable at least one of BUILD_TUI or BUILD_CLI.")
endif()

# -------------------------------------------------------------------
# Testing
# -------------------------------------------------------------------
if(BUILD_TESTING)
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
        message(FATAL_ERROR "GoogleTest submodule missing. Run: git submodule update --init --recursive")
    endif()

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(third_party/googletest)
    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES "tests/unit/*.cpp" "tests/integration/*.cpp")
    list(REMOVE_ITEM TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/test_main.cpp")

    add_executable(rwal_tests tests/test_main.cpp ${TEST_SOURCES})
    target_compile_features(rwal_tests PRIVATE cxx_std_20)
    target_include_directories(rwal_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/tests/mocks
        ${PROJECT_SOURCE_DIR}/tests/fixtures
    )
    target_link_libraries(rwal_tests PRIVATE rwal-core gtest_main gmock_main)

    include(GoogleTest)
    gtest_discover_tests(rwal_tests)
endif()

# -------------------------------------------------------------------
# Packaging
# -------------------------------------------------------------------
set(CPACK_PACKAGE_NAME "rwal")
set(CPACK_PACKAGE_VENDOR "Aloncie")
set(CPACK_PACKAGE_CONTACT "Aloncie@proton.me")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-alpha")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Wallpaper manager for KDE, GNOME, and Hyprland")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Aloncie <Aloncie@proton.me>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libcurl4, libstdc++6, libqt5core5a, libqt5widgets5, libqt5dbus5, libncurses6")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_GENERATOR "DEB;RPM;TGZ")
include(CPack)
