add_library(Executor Executor.cpp Task.cpp PerfModel.cpp)

target_link_libraries(Executor Common boost_system boost_iostreams)
