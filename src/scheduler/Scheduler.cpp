#include <Scheduler.hpp>

#include <iostream>
#include <cstring>

using namespace std;

void Scheduler::handleRequest(msg_t& request, msg_t& response) {
  cout << "Scheduler::movingAverage" << endl;
  cout << "Sending request to dispatcher now" << endl;

  if (request.msgId != MSG_DONE) {
    // don't terminate the connection between the scheduler and dispatcher
    send(&request, request.sizeBytes());

    // wait for reply back
    // XXX determine buffer size dynamically
    char buffer[1024];
    bzero(buffer, 1024);
    int n = read(Client::sockfd, buffer, 1024);

    msg_t* rsp = (msg_t*)buffer;
    response.msgId = rsp->msgId;
    response.dataSize = rsp->dataSize;
    response.paramsSize = 0;
    memcpy(response.data, rsp->data, rsp->sizeBytes());
  } else {
    response.msgId = MSG_ACK;
    response.dataSize = 0;
    response.paramsSize = 0;
  }
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
