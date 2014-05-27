add_library(Common Client.cpp Job.cpp MultiThreadedTCPServer.cpp Resource.cpp Implementation.cpp utils.cpp)
target_link_libraries (Common Pbuf pthread boost_system boost_program_options boost_thread boost_serialization boost_chrono dl)
