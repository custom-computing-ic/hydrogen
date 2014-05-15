#include <Allocations.hpp>
#include <Scheduler.hpp>
void Allocations::serviceAllocations(Scheduler &s) {
  auto j = jobs.begin();
  for(;j != jobs.end(); j++) {
    *j = move(s.removeFromQ(s.getQueuePointer("readyQ"), move(*j)));
    s.addToRunQ(move(*j));
  } 
}


void Allocations::returnResources(Scheduler& s) {
/*
 auto j = a.jobs.begin();
  for (;j != a.jobs.end(); j++) {
    auto r = (*j)->getAllocdRes()->begin();
    for (;r !=  (*j)->getAllocdRes()->end(); r++) {
      resPool->push_back(  move(*r)  );
    }
  }

*/
}
