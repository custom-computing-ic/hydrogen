#include "client.hpp"

#include <iostream>

#include <scheduler>

int movingAverage() {
  using namespace std;
  cout << "Doing " << endl;
  const Scheduler &scheduler = Scheduler();
  scheduler.movingAverage();
}
