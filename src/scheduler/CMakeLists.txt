add_library(Scheduler
	Scheduler.cpp
	algs.cpp
	Allocations.cpp
	ElasticityManager.cpp)

target_link_libraries (Scheduler Common)
