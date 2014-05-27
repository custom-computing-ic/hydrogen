add_library(Pbuf packet.pb.cc message.cpp)
target_link_libraries(Pbuf protobuf)
