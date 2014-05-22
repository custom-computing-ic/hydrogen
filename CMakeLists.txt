cmake_minimum_required (VERSION 2.6)
project (elastic-dfe)
SET (BOOST_MIN_VERSION "1.49.0")

# XXX This is a really bad hack...
SET (TCLAPDIR "/homes/mt3410/elasticcloud-api/src/include")

FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Fatal error: Boost (version >= 1.49) required.")
endif (NOT Boost_FOUND)

find_package(OpenMP)
if (OPENMP_FOUND)
  message(STATUS "OpenMP supported, compiling parllel bitstreams!")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(BITSTREAM_DIR "${PROJECT_SOURCE_DIR}/src/bitstreams")
SET(USE_BITSTREAMS "false")

# --- Bitstream libraries ---
if (EXISTS ${BITSTREAM_DIR} AND IS_DIRECTORY ${BITSTREAM_DIR})
  message (STATUS "Found bitstreams, checking for required compilers...")
  if (DEFINED ENV{MAXCOMPILERDIR} AND DEFINED ENV{MAXELEROSDIR})
      message(STATUS "MAXCOMPILERDIR and MAXELEROSDIR are set, adding includes!")
      include_directories (${BITSTREAM_DIR})
      include_directories ($ENV{MAXCOMPILERDIR}/include)
      include_directories ($ENV{MAXCOMPILERDIR}/include/slic)
      include_directories ($ENV{MAXELEROSDIR}/include)
      link_directories($ENV{MAXCOMPILERDIR}/lib)
      link_directories($ENV{MAXELEROSDIR}/lib)
      add_subdirectory (src/bitstreams)
      add_definitions("-DUSEDFE")
      SET(USE_BITSTREAMS "true")
  else()
    message(STATUS "MAXCOMPILERDIR not set!")
  endif()

else()
  message (STATUS "No bitstreams found, compiling with DFE implementations disabled.")
endif ()

include_directories("${TCLAPDIR}")

include_directories ("${PROJECT_SOURCE_DIR}/src/common")
add_subdirectory (src/common)

include_directories ("${PROJECT_SOURCE_DIR}/src/dispatcher")
add_subdirectory (src/dispatcher)

include_directories ("${PROJECT_SOURCE_DIR}/src/scheduler")
add_subdirectory (src/scheduler)

include_directories ("${PROJECT_SOURCE_DIR}/src/client")
add_subdirectory (src/client)

include_directories ("${PROJECT_SOURCE_DIR}/src/executor")
add_subdirectory (src/executor)

include_directories ("${PROJECT_SOURCE_DIR}/test")
add_subdirectory (test)

set (EXTRA_LIBS ${EXTRA_LIBS} Dispatcher Scheduler Client)

# Dispatcher Executable
add_executable (dispatcher src/dispatcher/main.cxx)
target_link_libraries (dispatcher Dispatcher)

# Scheduler Executable
add_executable (scheduler src/scheduler/main.cxx)
target_link_libraries (scheduler Scheduler)

# Executor Executable
add_executable  (executor src/executor/main.cxx)
target_link_libraries (executor Executor )

# Tests - Can use these as client executables
add_executable (MovingAverageOneDfe test/MovingAverageOneDfe.cpp)
target_link_libraries (MovingAverageOneDfe ${EXTRA_LIBS} TestLib)

add_executable (MovingAverageOneDfePriority test/MovingAverageOneDfePriority.cpp)
target_link_libraries (MovingAverageOneDfePriority ${EXTRA_LIBS} TestLib)

add_executable (MultiUserSingleJobType test/MultiUserSingleJobType.cpp)
target_link_libraries (MultiUserSingleJobType ${EXTRA_LIBS} TestLib)

add_executable (DispatcherTest test/DispatcherTest.cpp)
target_link_libraries (DispatcherTest ${EXTRA_LIBS} TestLib)

enable_testing ()
add_test ("DispatcherTest" DispatcherTest)
add_test ("MovingAverageOneDfe" MovingAverageOneDfe)
add_test ("MultiUserSingleJobType" MultiUserSingleJobType)
