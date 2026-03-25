cmake_minimum_required(VERSION 3.10)

project(rwal VERSION 0.5.0 LANGUAGES CXX)

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

# Dependencies
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Widgets)
find_package(Curses REQUIRED)

add_subdirectory(src/wallpaper)

# Main App Library Sources
set(SOURCES
    src/AppController/AppController.cpp
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
)

set(HEADERS
    src/AppController/AppController.hpp
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
)

# Create rwal_lib
add_library(rwal_lib STATIC ${SOURCES} ${HEADERS})

target_include_directories(rwal_lib PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

# Linking
target_link_libraries(rwal_lib PUBLIC
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Qt5::DBus
    Qt5::Core
    Qt5::Widgets
    ${CURSES_LIBRARIES}
    wallpaper_lib 
)

# Executable
add_executable(rwal src/main.cpp)
target_link_libraries(rwal PRIVATE rwal_lib)

# Installation
install(TARGETS rwal RUNTIME DESTINATION bin)

# --- SECTION: Testing (with safety checker) ---
if(BUILD_TESTING)
    if(EXISTS "${PROJECT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
        add_subdirectory(third_party/googletest)
        enable_testing()
        
        add_executable(rwal_tests
            tests/test_main.cpp
            tests/test_string_utils.cpp
        )

        target_link_libraries(rwal_tests PRIVATE rwal_lib GTest::gtest_main)
        target_include_directories(rwal_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)

        include(GoogleTest)
        gtest_discover_tests(rwal_tests)
    else()
        message(WARNING "GoogleTest submodules not found. Tests will not be built. Run 'git submodule update --init' to fix this.")
    endif()
endif()

# --- SECTION: Packaging ---
set(CPACK_PACKAGE_NAME "rwal")
set(CPACK_PACKAGE_VENDOR "Aloncie")
set(CPACK_PACKAGE_CONTACT "Aloncie@proton.me")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-alpha")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Wallpaper manager for KDE and GNOME")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Aloncie <Aloncie@proton.me>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libcurl4, libstdc++6, libqt5core5a, libqt5widgets5, libqt5dbus5, libncurses6")

set(CPACK_GENERATOR "DEB;RPM;TGZ")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)
