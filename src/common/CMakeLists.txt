add_library(Common message.cpp Server.cpp Client.cpp Job.cpp MultiThreadedTCPServer.cpp Resource.cpp)

target_link_libraries (Common pthread boost_system boost_thread)
