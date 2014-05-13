#include <Scheduler.hpp>

#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;

void Scheduler::addToReadyQ(msg_t& request, msg_t& response) {
  Client::send(&request, request.sizeBytes());
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

}



void Scheduler::handleRequest(msg_t& request, msg_t& response) {
  cout << "Scheduler recieved request msgID[" <<  request.msgId << "]" << endl;

  switch(request.msgId) {
    case MSG_DONE:
    response.msgId = MSG_ACK;
    response.dataSize = 0;
    response.paramsSize = 0;
    break;
    case MSG_ACK:
      break;
    default:
      cout << "Request added to runQ" << endl ;
      this->addToReadyQ(request,response);
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
