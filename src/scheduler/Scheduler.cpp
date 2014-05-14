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



void Scheduler::addToRunQ(Job& j) {
  j.setDispatchTime(curTime);
  estimateFinishTime(j);
  runQ->push_back(make_shared<Job>(j));
}


void Scheduler::returnToReadyQ(Job& j, int pos) {
  auto it = readyQ->begin();
  for (int i = 0; i < pos && it < readyQ->end(); i++, it++);

  readyQ->insert(it,make_shared<Job>(j));
}

int Scheduler::allocate(Job& j, int max_res, int min_res) {
  if (resPool->size() >= min_res) {
    while (max_res-- > 0 && resPool->size() > 0) {
      j.allocate(res->getContainer()->back());
      resPool->pop_back();
    }
    std::cout << "Allocated " << j.getAllocdRes()->size()
              << " Resources to " << j << "\n";
    return j.noAllocdRes()->size();
  } else {
    return -1;
  }
}





/* Server Handling */
void Scheduler::handleRequest(msg_t& request, msg_t& response) {
  cout << "Scheduler recieved request msgID[" <<  request.msgId << "]" << endl;
  //TODO: Lookup requestID/Implementation ID in a map and return error if not found
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
  for (auto it = resPool->begin(); it != resPool->end(); it++)
    (*it)->start();
  Server::start();
}

void Scheduler::stop() {
  for (auto it = resPool->begin(); it != resPool->end(); it++)
    (*it)->start();  
  Server::stop();
}









