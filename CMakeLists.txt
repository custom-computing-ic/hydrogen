cmake_minimum_required (VERSION 2.6)
project (elastic-dfe)

SET (BOOST_ROOT "/usr/local/boost_1_55_0")
SET (BOOST_MIN_VERSION "1.55.0")
SET (BOOST_INCLUDEDIR "/usr/local/boost_1_55_0/include")
SET (BOOST_LIBRARYDIR "/usr/local/boost_1_55_0/lib")

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Fatal error: Boost (version >= 1.55) required.")
else()
  message(STATUS "Setting up boost")
  message(STATUS "   ${Boost_INCLUDE_DIRS}")
  message(STATUS "   ${Boost_LIBRARY_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif (NOT Boost_FOUND)

find_package(OpenMP)
if (OPENMP_FOUND)
  message(STATUS "OpenMP supported, compiling parallel bitstreams!")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --std=c++11 -g")
#SET(CMAKE_CXX_COMPILER "clang")
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
#Needed for protobuf based control messages
#include_directories ("${PROJECT_SOURCE_DIR}/src/common/protobuf")
#add_subdirectory (src/common/protobuf)

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

include_directories ("${PROJECT_SOURCE_DIR}/test/common")
add_subdirectory (test/common)

include_directories ("${PROJECT_SOURCE_DIR}/test/common")
add_subdirectory (test/dispatcher)

set (EXTRA_LIBS ${EXTRA_LIBS} Dispatcher Scheduler Client Common)

# Dispatcher Executable
add_executable (dispatcher src/dispatcher/main.cxx)
target_link_libraries (dispatcher Dispatcher)

# Scheduler Executable
add_executable (scheduler src/scheduler/main.cxx)
target_link_libraries (scheduler Scheduler)

# Executor Executable
add_executable  (executor src/executor/main.cxx)
target_link_libraries (executor Executor ${EXTRA_LIBS})

# Tests - Can use these as client executables
# Add resource Test - needs protobufs and currently segfaults. should find out
# why
#add_executable (AddResource test/AddResource.cpp)
#target_link_libraries(AddResource ${EXTRA_LIBS} TestLib)

add_executable (MovingAverageOneDfe test/MovingAverageOneDfe.cpp)
target_link_libraries (MovingAverageOneDfe ${EXTRA_LIBS} TestLib)

add_executable (MovingAverageOneDfeSmall test/MovingAverageOneDfeSmall.cpp)
target_link_libraries (MovingAverageOneDfeSmall ${EXTRA_LIBS} TestLib)

add_executable (MovingAverageOneDfeMedium test/MovingAverageOneDfeMedium.cpp)
target_link_libraries (MovingAverageOneDfeMedium ${EXTRA_LIBS} TestLib)

add_executable (MovingAverageOneDfeLarge test/MovingAverageOneDfeLarge.cpp)
target_link_libraries (MovingAverageOneDfeLarge ${EXTRA_LIBS} TestLib)

add_executable (MovingAverageOneDfePriority test/MovingAverageOneDfePriority.cpp)
target_link_libraries (MovingAverageOneDfePriority ${EXTRA_LIBS} TestLib)

add_executable (MultiUserSingleJobType test/MultiUserSingleJobType.cpp)
target_link_libraries (MultiUserSingleJobType ${EXTRA_LIBS} TestLib)

add_executable (MultiUserSingleJobTypeSmall test/MultiUserSingleJobTypeSmall.cpp)
target_link_libraries (MultiUserSingleJobTypeSmall ${EXTRA_LIBS} TestLib)

add_executable (MultiUserSingleJobTypeLarge test/MultiUserSingleJobTypeLarge.cpp)
target_link_libraries (MultiUserSingleJobTypeLarge ${EXTRA_LIBS} TestLib)

add_executable (DispatcherTest test/dispatcher/DispatcherTest.cpp)
target_link_libraries (DispatcherTest ${EXTRA_LIBS} TestLib)

add_executable (OptionPricing test/OptionPricingTest.cpp)
target_link_libraries (OptionPricing ${EXTRA_LIBS} TestLib)

add_executable (OptionPricingJlo test/OptionPricingJloTest.cpp)
target_link_libraries (OptionPricingJlo ${EXTRA_LIBS} TestLib)

add_executable (TerminateServer test/TerminateServer.cpp)
target_link_libraries (TerminateServer ${EXTRA_LIBS} TestLib)

add_executable(ConcurrentDequeTest test/common/ConcurrentDequeTest.cpp)
target_link_libraries(ConcurrentDequeTest ${EXTRA_LIBS} TestLib)

enable_testing ()

add_test ("DispatcherTest" DispatcherTest)

# add_test ("MovingAverageOneDfe" MovingAverageOneDfe)
# add_test ("MovingAverageOneDfeSmall" MovingAverageOneDfeSmall)
# add_test ("MovingAverageOneDfeMedium" MovingAverageOneDfeMedium)
# add_test ("MovingAverageOneDfeLarge" MovingAverageOneDfeLarge)
# add_test ("MultiUserSingleJobType" MultiUserSingleJobType)

# add_test ("MultiUserSingleJobTypeSmall" MultiUserSingleJobTypeSmall)
# add_test ("MultiUserSingleJobTypeLarge" MultiUserSingleJobTypeLarge)

# add_test ("OptionPricing" OptionPricing)
# add_test ("OptionPricingJlo" OptionPricingJlo)

# add_test ("TerminateServer" TerminateServer)

# Common tests
add_test ("ConcurrentDequeTest" ConcurrentDequeTest)