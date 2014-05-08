#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <Dispatcher.hpp>

class Scheduler {

  Dispatcher dispatcher;

public:

  Scheduler() {}

  void movingAverage();
};

#endif /* _SCHEDULER_H_ */
