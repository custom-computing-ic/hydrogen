cmake_minimum_required (VERSION 2.6)
project (elastic-dfe)
SET (BOOST_MIN_VERSION "1.49.0")


FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Fatal error: Boost (version >= 1.49) required.\n")
endif (NOT Boost_FOUND)


SET(CMAKE_CXX_COMPILER  "/usr/bin/clang++")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Weverything")

include_directories ("${PROJECT_SOURCE_DIR}/src/common")
add_subdirectory (src/common)

include_directories ("${PROJECT_SOURCE_DIR}/src/dispatcher")
add_subdirectory (src/dispatcher)

include_directories ("${PROJECT_SOURCE_DIR}/src/scheduler")
add_subdirectory (src/scheduler)

include_directories ("${PROJECT_SOURCE_DIR}/src/client")
add_subdirectory (src/client)

set (EXTRA_LIBS ${EXTRA_LIBS} Dispatcher Scheduler Client)

# Dispatcher Executable
add_executable (dispatcher src/dispatcher/main.cxx)
target_link_libraries (dispatcher Dispatcher pthread boost_system boost_thread)

# Scheduler Executable
add_executable (scheduler src/scheduler/main.cxx)
target_link_libraries (scheduler Scheduler)


# Tests - Can use these as client executables
add_executable (MovingAverageOneDfe test/integration/movingAverageOneDfe/main.cpp)
target_link_libraries (MovingAverageOneDfe ${EXTRA_LIBS})

add_executable (MultiUserSingleJobType test/integration/MultiUserSingleJobType.cpp)
target_link_libraries (MultiUserSingleJobType ${EXTRA_LIBS})

add_executable (DispatcherTest test/DispatcherTest.cpp)
target_link_libraries (DispatcherTest ${EXTRA_LIBS})

enable_testing ()
add_test ("DispatcherTest" DispatcherTest)
add_test ("MovingAverageOneDfe" MovingAverageOneDfe)
add_test ("MultiUserSingleJobType" MultiUserSingleJobType)
