#include "client.hpp"

#include <iostream>

#include <Scheduler.hpp>

void movingAverage() {
  using namespace std;
  cout << "client::movingAverage" << endl;
  Scheduler scheduler;
  scheduler.movingAverage();
}
