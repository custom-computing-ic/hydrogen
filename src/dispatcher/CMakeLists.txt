add_library(Dispatcher Dispatcher.cpp DispatcherServer.cpp)

target_link_libraries(Dispatcher Common pthread boost_system boost_thread)
