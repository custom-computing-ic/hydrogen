cmake_minimum_required (VERSION 2.6)
project (elastic-dfe)

include_directories ("${PROJECT_SOURCE_DIR}/src/dispatcher")
add_subdirectory (src/dispatcher)
set (EXTRA_LIBS ${EXTRA_LIBS} Dispatcher)

add_executable(Main main.cxx)
target_link_libraries (Main  ${EXTRA_LIBS})