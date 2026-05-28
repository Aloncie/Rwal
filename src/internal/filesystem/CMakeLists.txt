set(PLATFORM_FS_SOURCES NativeFileSystem.cpp)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND PLATFORM_FS_SOURCES LinuxFileSystem.cpp)
    message(STATUS "Detected Linux system. Using Linux filesystem")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND PLATFORM_FS_SOURCES WindowsFileSystem.cpp)
    message(STATUS "Detected Windows system. Using Windows filesystem")
endif()

add_library(filesystem_lib
    FileSystemFactory.cpp
    ${PLATFORM_FS_SOURCES}
)
target_include_directories(filesystem_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
