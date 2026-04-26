add_library(scheduler-lib STATIC SchedulerFactory.cpp)

target_include_directories(scheduler-lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

if (UNIX AND NOT APPLE)
	target_sources(scheduler-lib PRIVATE LinuxSystemScheduler.cpp)
	target_link_libraries(scheduler-lib PRIVATE Qt6::Core)
	message(STATUS "Detected Linux system. Using Linux system scheduler")
elseif(WINDOWS)
	target_sources(scheduler-lib PRIVATE WindowsSystemScheduler.cpp)
	message(STATUS "Detected Windows system. Using Windows system scheduler")
else()
	target_sources(scheduler-lib PRIVATE FallbackSystemScheduler.cpp)
	message(STATUS "Unknown system. Using fallback system scheduler")
endif()

