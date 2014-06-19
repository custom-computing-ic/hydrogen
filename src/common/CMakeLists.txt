add_library(Common Client.cpp Job.cpp MultiThreadedTCPServer.cpp Resource.cpp
        Implementation.cpp utils.cpp message.cpp StatsScreen.cpp)

#Need to add Pbuf for protobuf control messages.. see subdir protobuf/ cmake
#file
target_link_libraries (Common pthread boost_system boost_program_options
        boost_thread boost_serialization boost_chrono dl ncurses)
