#include "ElasticityManager.hpp"

#include <iostream>
#include "Scheduler.hpp"
#include "Logging.hpp"

using namespace std;

void ElasticityManager::updateResourcePool(const Scheduler &scheduler,
					   Allocations &allocations) const {

  double jloMetric = allocations.getJLOMetric();

  LOG(debug) << "JLOMetric: " << jloMetric;

}
