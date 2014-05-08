cmake_minimum_required (VERSION 2.6)
project (elastic-dfe)

include_directories ("${PROJECT_SOURCE_DIR}/src/dispatcher")
add_subdirectory (src/dispatcher)

include_directories ("${PROJECT_SOURCE_DIR}/src/scheduler")
add_subdirectory (src/scheduler)

include_directories ("${PROJECT_SOURCE_DIR}/src/client")
add_subdirectory (src/client)

set (EXTRA_LIBS ${EXTRA_LIBS} Dispatcher Scheduler Client)

add_executable (MovingAverageOneDfe test/integration/movingAverageOneDfe/main.cpp)
target_link_libraries (MovingAverageOneDfe ${EXTRA_LIBS})

enable_testing ()
add_test ("MovingAverageOneDfe" MovingAverageOneDfe)
