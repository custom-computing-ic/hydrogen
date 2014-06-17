ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
SET(Boost_USE_STATIC_LIBS ON)

add_library(Client client_api.cpp)

target_link_libraries(Client Common boost_log)