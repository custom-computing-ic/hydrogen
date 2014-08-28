#include "ElasticityManager.hpp"

#include <iostream>
#include "Scheduler.hpp"
#include "Logging.hpp"

using namespace std;

void ElasticityManager::updateResourcePool(Scheduler &scheduler,
					   Allocations &allocations) const {

  LOG(debug) << "Updating JLO Metric: ";
  double jloMetric = allocations.getJLOMetric();
  LOG(debug) << "JLOMetric: " << jloMetric;

  double beta = 1;
  if (jloMetric < 0) {
    LOG(debug) << "JLO missed, adding resources" << endl;
    scheduler.provisionResource();
  } else if (jloMetric > beta) {
    LOG(debug) << "JLO exceeded, removing resources" << endl;
    scheduler.deprovisionResource();
  }
}
