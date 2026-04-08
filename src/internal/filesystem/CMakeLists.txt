if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(PLATFORM_FS_SOURCE LinuxFileSystem.cpp)
endif()

add_library(filesystem_lib
	FileSystemFactory.cpp
	${PLATFORM_FS_SOURCE}
)

target_include_directories(filesystem_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filesystem_lib PUBLIC Qt5::Core)

