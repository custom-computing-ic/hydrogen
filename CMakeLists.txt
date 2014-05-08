cmake_minimum_required (VERSION 2.6)
project (elastic-dfe)

include_directories ("${PROJECT_SOURCE_DIR}/src/dispatcher")
add_subdirectory (src/dispatcher)

include_directories ("${PROJECT_SOURCE_DIR}/src/scheduler")
add_subdirectory (src/scheduler)

include_directories ("${PROJECT_SOURCE_DIR}/src/client")
add_subdirectory (src/client)

set (EXTRA_LIBS ${EXTRA_LIBS} Scheduler Dispatcher Client)

add_executable(Main main.cxx)
target_link_libraries (Main  ${EXTRA_LIBS})