#ifndef _ELASTICITYMANAGER_H_
#define _ELASTICITYMANAGER_H_

class Scheduler;

class ElasticityManager {

public:
  ElasticityManager() {}

  /** Update the size of the schedueler resource pool based on the
      jloMetric. */
  void updateResourcePool(Scheduler& scheduler);
};

#endif /* _ELASTICITYMANAGER_H_ */
