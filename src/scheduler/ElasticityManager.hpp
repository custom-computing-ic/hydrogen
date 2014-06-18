#ifndef _ELASTICITYMANAGER_H_
#define _ELASTICITYMANAGER_H_

#include "Allocations.hpp"

class Scheduler;

class ElasticityManager {

public:
  ElasticityManager() {}

  /** Update the size of the schedueler resource pool based on the
      jloMetric. */
  void updateResourcePool(const Scheduler &, Allocations &) const;
};

#endif /* _ELASTICITYMANAGER_H_ */
