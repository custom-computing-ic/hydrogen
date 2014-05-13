#include <Scheduler.hpp>

#include <iostream>

using namespace std;

void Scheduler::handleRequest(msg_t& request, msg_t& response) {
  cout << "Scheduler::movingAverage" << endl;
  cout << "Sending request to dispatcher now" << endl;

  if (request.msgId != MSG_DONE)
    // don't terminate the connection between the scheduler and dispatcher
    send(&request, request.sizeBytes());
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
