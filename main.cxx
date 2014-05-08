#include <iostream>

#include <Dispatcher.hpp>
#include <Scheduler.hpp>
#include <client.hpp>

using namespace std;

int main() {
  Dispatcher d;
  d.movingAverage();

  Scheduler s;
  s.movingAverage();

  movingAverage();
}
