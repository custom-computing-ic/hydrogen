#include <Allocations.hpp>
#include <Scheduler.hpp>
#include <Logging.hpp>

void Allocations::serviceAllocations(Scheduler &s) {
  LOG(debug) << "Service allocations";
  auto j = jobs.begin();
  for (; j != jobs.end(); j++) {
    s.removeJobFromReadyQ(*j);
    s.claimResources(*j);
    s.addToRunQ(*j);
  }
}

void Allocations::returnResources(Scheduler &s) {
  LOG(debug) << "\t\t* Return resources";
  auto it = jobs.begin();
  for (; it != jobs.end(); it++) {
    s.returnResources(std::get<1>(**it));
  }
}
