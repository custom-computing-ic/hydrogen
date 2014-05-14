#include <Scheduler.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;
//TODO[mtottenh] Finish implementing the rest of the scheduler class

void Scheduler::addToReadyQ(msg_t& request, msg_t& response) {
  resPool->front()->send(&request, request.sizeBytes());
  // wait for reply back
  // XXX determine buffer size dynamically
  char buffer[1024];
  bzero(buffer, 1024);
  int n = resPool->front()->read(buffer, 1024);

  msg_t* rsp = (msg_t*)buffer;
  response.msgId = rsp->msgId;
  response.dataSize = rsp->dataSize;
  response.paramsSize = 0;
  memcpy(response.data, rsp->data, rsp->dataBytes());
}


//TODO[mottenh]: Actualy make this communicate with the allocated dispatchers..
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


int Scheduler::estimateFinishTime(Job& j) {
  return -1;
}
int Scheduler::numLateJobs() {
  return -1;
}
void Scheduler::updateState() {

}

void Scheduler::dumpInfo() {

}
void Scheduler::printQInfo(const char*, JobQueuePtr, bool) {

}
void Scheduler::reclaimResources() {

}
void Scheduler::schedule() {

}
void Scheduler::schedule(int) {

}
Allocations Scheduler::schedule(int,bool) {

}

int Scheduler::allocate(Job& j, int max_res, int min_res) {
  if (resPool->size() >= min_res) {
    while (max_res-- > 0 && resPool->size() > 0) {
      j.allocate(resPool->back());
      resPool->pop_back();
    }
    std::cout << "Allocated " << j.noAllocdRes()
              << " Resources to " << j << "\n";
    return j.noAllocdRes();
  } else {
    return -1;
  }
}


void Scheduler::deallocate(Job& j) {

}
void Scheduler::realloc(Job& j) {

}
void Scheduler::serviceAllocations(Allocations &a) {

}
void Scheduler::returnResources(Allocations &a) {

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
    default:
      cout << "Request added to readyQ" << endl ;
      this->addToReadyQ(request,response);
      break;
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








