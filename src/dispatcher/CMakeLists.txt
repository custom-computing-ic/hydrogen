add_library(Dispatcher Dispatcher.cpp DispatcherServer.cpp)

if (${USE_BITSTREAMS} STREQUAL "true")
  message(STATUS "Linking Dispatcher with Bitstreams")
  target_link_libraries(Dispatcher Common Bitstreams pthread boost_system boost_thread)
else()
  target_link_libraries(Dispatcher Common pthread boost_system boost_thread)
endif()
