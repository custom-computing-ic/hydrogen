#include <Scheduler.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;



template <typename T> 
void Scheduler::removeFromQ(typename ContainerPtr<T>::deque jq, T j) {

  typename ContainerPtr<T>::deque preserve_list;
//  preserve_list = new std::deque<T>();
  auto a = jq->begin();
  for(;a != jq->end(); a++) {
    if ((*a)->getId() != j->getId()) {
      preserve_list->push_back(*a);
    } 
  } 
  jq = preserve_list;

}
//TODO[mtottenh] Finish implementing the rest of the scheduler class
void Scheduler::defaultHandler(msg_t& request, msg_t& response) {
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

/* TODO[mtottenh]: insert wrapper here to construct a job from a message when
  the job class is finished.
 */
void Scheduler::addToReadyQ(msg_t& request, msg_t& response) {
//  Job j(request);
  readyQ->push_back(make_shared<Job>(request));
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


/*
 * TODO[mtottenh]: Check implementations of getDispatchTime/IssueTime with timevals
 * in the minites/seconds range not just random floats.. 
 */

int Scheduler::numLateJobs() {
  int sum = 0;
  auto it = finishedJobs->begin();
  for(;it != finishedJobs->end(); it++) {
    sum = (*it)->getDispatchTime() - (*it)->getIssueTime() > 1 ? sum+1 : sum;
  }
  return sum;
}

void Scheduler::updateState() {

}

void Scheduler::dumpInfo() {

}
void Scheduler::printQInfo(const char*, JobQueuePtr, bool) {

}
//TODO[mtottenh]: Add this to the header file. This could be 
// dangerous.. it seems to remove all the resources in the runQ and return them to the pool.
// use with extreme caution
void Scheduler::reclaimResources() {

  auto j = runQ->begin();

  for (;j != runQ->end(); j++) {
    auto a = (*j)->getAllocdRes()->begin();
    for (;a != (*j)->getAllocdRes()->end(); a++) {
      resPool->push_back(*a);
    }
  }

}


//TODO[mtottenh]: Check if this is ever used. I think it can be removed.
void Scheduler::schedule() {

}

//TODO[mtottenh]: Collapse the below back into 1 function. needless code duplication here.
void Scheduler::schedule(int choice) {
  Allocations a;
  if (resPool->size() > 0) {
    a = this->algVec[choice](*this);
    serviceAllocations(a);
  }
}
Allocations Scheduler::schedule(int choice, bool flag) {
  flag = false;
  Allocations a;
  if (resPool->size() > 0) {
    a = this->algVec[choice](*this);
  }
 return a;
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

//TODO[mtottenh]:Check this functions as intended. I'm not sure about getAllocdRes()->***/
void Scheduler::deallocate(Job& j) {
  std::cout << "De-allocating " << j  
            << "{" << j.noAllocdRes() << "}" 
            << "|" << resPool->size() << "|"; 
  auto r = j.getAllocdRes()->begin(); 
  for (; r != j.getAllocdRes()->end(); r++) { 
    resPool->push_back(*r); 
  } 
  j.getAllocdRes()->clear(); 
  std::cout << "(" << resPool->size() << ")\n";

}

/*TODO[mtottenh]: Is this ever used? */
void Scheduler::realloc(Job& j) {
  auto r = j.getAllocdRes()->begin();
  for (; r != j.getAllocdRes()->end(); r++) {
     this->removeFromQ(resPool, *r);
  }

}

//TODO[mtottenh]: add code to communicate with dispatcher here...
void Scheduler::serviceAllocations(Allocations &a) {
  auto j = a.jobs.begin();
  for(;j != a.jobs.end(); j++) {
    this->removeFromQ(readyQ, make_shared<Job>(*j));
    this->addToRunQ(*j);
  }

}
void Scheduler::returnResources(Allocations &a) {
  auto j = a.jobs.begin();
  for (;j != a.jobs.end(); j++) {
    auto r = j->getAllocdRes()->begin();
    for (;r <  j->getAllocdRes()->end(); r++) {
      resPool->push_back(*r);
    }
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
    case MSG_Moving_AVG:
      addToReadyQ(request,response);
      break;
    default:
        defaultHandler(request,response);
//      cout << "Request added to readyQ" << endl ;
//      this->addToReadyQ(request,response);
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








