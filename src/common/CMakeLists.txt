add_library(Common message.cpp Client.cpp Job.cpp MultiThreadedTCPServer.cpp Resource.cpp Implementation.cpp)

target_link_libraries (Common pthread boost_system boost_thread dl)
