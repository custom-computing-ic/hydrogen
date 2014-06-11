#include <Allocations.hpp>
#include <Scheduler.hpp>
void Allocations::serviceAllocations(Scheduler &s) {
  std::cout << "Allocations::serviceAllocations()\n";
  auto j = jobs.begin();
  for(;j != jobs.end(); j++) {
    s.removeJobFromReadyQ(*j);
    s.claimResources(*j);
    s.addToRunQ(*j);
  }

}


void Allocations::returnResources(Scheduler& s) {
  std::cout << "\t\tAllocations::returnResources()" << std::endl;
  auto it = jobs.begin();
  for ( ; it != jobs.end(); it++) {
       s.returnResources(std::get<1>(**it));
  }
}
