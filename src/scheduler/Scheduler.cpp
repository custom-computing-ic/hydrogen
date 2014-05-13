#include <Scheduler.hpp>

#include <iostream>

using namespace std;

void Scheduler::handleRequest(msg_t& request) {
  cout << "Scheduler::movingAverage" << endl;
  cout << "Sending request to dispatcher now" << endl;
  send(&request, sizeof(msg_t) + request.dataSize * sizeof(int));
}

void Scheduler::start() {
  // Order matters, since the server blocks waiting for requests
  Client::start();
  Server::start();
}

void Scheduler::stop() {
  Client::stop();
  Server::stop();
}
