#include <Scheduler.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>
#define MODE_MANAGED 4
using namespace std;




int Scheduler::getJobStatus(int jobID) {
  //check runQ

  for (auto it = runQ->begin(); it != runQ->end(); it++) {
    if ((*it)->getId() == jobID) {
      return (*it)->getStatus();
    }
  }

  return -1;
  //check readyQ
  //check finishedQ

}

//TODO[mtottenh] Finish implementing the rest of the scheduler class
void Scheduler::defaultHandler(msg_t& request, msg_t& response, int responseSize) {
  resPool->front()->send(&request);

  // wait for reply back
  char buffer[responseSize];
  memset(buffer, 0, responseSize);
  int n = resPool->front()->read(buffer, responseSize);

  cout << "Default handler " << endl;
  request.print();
  response.print();

  msg_t* rsp = (msg_t*)buffer;
  rsp->print();
  response.msgId = rsp->msgId;
  response.dataSize = rsp->dataSize;
  response.paramsSize = 0;
  memcpy(response.data, rsp->data, rsp->dataBytes());

  cout << "DefHandler Response" << endl;
  response.print();
}

/* TODO[mtottenh]: insert wrapper here to construct a job from a message when
  the job class is finished.
 */
int Scheduler::addToReadyQ(msg_t& request) {
  readyQ->push_back(unique_ptr<Job>(new Job(request,this->getNextId())));
  return readyQ->back()->getId();
}


//TODO[mottenh]: Actualy make this communicate with the allocated dispatchers..
void Scheduler::addToRunQ(std::unique_ptr<Job> j) {
  cout << "Adding " << *j << "to the runQ\n";
  j->setDispatchTime(curTime);
  j = move (estimateFinishTime(move(j)));
  runQ->push_back( move(j));

}


void Scheduler::returnToReadyQ(JobPtr j, int pos) {
  auto it = readyQ->begin();
  for (int i = 0; i < pos && it < readyQ->end(); i++, it++);

  readyQ->insert(it,move(j));
}


Scheduler::JobPtr Scheduler::estimateFinishTime(JobPtr j) {
  //TODO[mtottenh]:Add code to estimate a job finish time
  return j;
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
      resPool->push_back(   move(*a)   );
    }
  }

}


//TODO[mtottenh]: Check if this is ever used. I think it can be removed.
void Scheduler::schedule() {

}

//TODO[mtottenh]: Collapse the below back into 1 function. needless code duplication here.
void Scheduler::schedule(int choice) {
  if (resPool->size() > 0) {
    this->algVec[choice](*this).serviceAllocations(*this);
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

Scheduler::JobPtr Scheduler::allocate(JobPtr j, int max_res, int min_res) {
  if (resPool->size() >= min_res) {
    while (max_res-- > 0 && resPool->size() > 0) {
      j->allocate( move(resPool->back()) );
      resPool->pop_back();
    }
    std::cout << "Allocated " << j->noAllocdRes()
              << " Resources to " << *j << "\n";
  }
  return j;
}

//TODO[mtottenh]:Check this functions as intended. I'm not sure about getAllocdRes()->***/
Scheduler::JobPtr Scheduler::deallocate(JobPtr j) {
  std::cout << "De-allocating " << *j
            << "{" << j->noAllocdRes() << "}"
            << "|" << resPool->size() << "|";
  auto r = j->getAllocdRes()->begin();
  for (; r != j->getAllocdRes()->end(); r++) {
    resPool->push_back( move(*r ));
  }
  j->getAllocdRes()->clear();
  std::cout << "(" << resPool->size() << ")\n";
  return j;
}

/*TODO[mtottenh]: Is this ever used? */
Scheduler::JobPtr Scheduler::realloc(JobPtr j) {
  auto r = j->getAllocdRes()->begin();
  for (; r != j->getAllocdRes()->end(); r++) {
     *r = move(removeFromQ(resPool, move( *r)));
  }
  return j;
}


//void Scheduler::returnResources(Allocations &a)


msg_t Scheduler::getJobResponse(int jobID) {
  msg_t response;
  char buffer[1024];
  bzero(buffer, 1024);
  for (auto job = runQ->begin(); job != runQ->end(); job++) {
    if ((*job)->getId() == jobID) {
      (*job)->getResponse(buffer,1024);
    }
  }

  msg_t* rsp = (msg_t*)buffer;
  response.msgId = rsp->msgId;
  response.dataSize = rsp->dataSize;
  response.paramsSize = 0;
  memcpy(response.data, rsp->data, rsp->dataBytes());
  return response;
}

/* Server Handling */
msg_t* Scheduler::handle_request(msg_t* request) {
  cout << "Scheduler recieved request msgID[" <<  request->msgId << "]" << endl;
  request->print();
  int jobID = -1;
  int jobStatus = -1;
  //TODO: Lookup requestID/Implementation ID in a map and return error if not found
  msg_t* response;
  response = msg_ack();
  switch(request->msgId) {
    case MSG_DONE:
      return msg_ack();
    case MSG_MOVING_AVG:
    default:
      // XXX TODO[paul-g]: need to actually determine data size here
      cout << "Request data size " << request->dataSize << endl;
      int sizeBytes = sizeof(msg_t) + sizeof(int) * request->dataSize;
      msg_t* response = (msg_t*)calloc(sizeBytes, 1);
      return  concurrentHandler(*request, *response, sizeBytes);
//      defaultHandler(*request, *response, sizeBytes);
//      return response;
      break;
  }
}

//Need a QStatus struct.

void Scheduler::schedLoop() {
  while (true) {
    //wait on readyQ or FinishedQ condvar
    boost::unique_lock<boost::mutex> lock(qMutex);
    //If we have no results or nothing to try and schedule wait
    while ( !QStatus.getReadyQStatus() && !QStatus.getFinishedQStatus() ) {
      QCondVar.wait(lock);
    } 
    //A job was deposited in the readyQ
    if (QStatus.getReadyQStatus() == true) {
      //attempt to schedule
      //TODO[mtottenh]: modify schedule to lock all Q's for now.
      if ( schedule() ) {
        // we managed to deposite some jobs in the runQ
        // tell the dispatcher that it has work todo
        QStatus.setRunQStatus(true);
        QCondVar.notify_all();
      } else {
        //No/not enough free resources for a schedule

      }

    }
    //A result was returned on the finishedQ
    if (QStatus.getFinishedQStatus() == true) {
      //communicate to main thread that jobs in the finished Q are done
      //finshedQ.notifyClients();
      QStatus.setFinishedQStatus(false);
    }
  }
}

void Scheduler::dispatcherLoop() {
  while (true) {

  }
}

msg_t*  Scheduler::concurrentHandler( msg_t &request, msg_t &response, int sizeBytes) {
    //creat (job,condvar) pair
    //enqueue(readyQ,(job,condvar))
    //block on job condition
    //wake up
    //dequeue(finishedQ,(job,condvar));
    //return job.response
}




void Scheduler::kickStartRunQ() {
  for (auto job = runQ->begin(); job != runQ->end(); job++)  {
    if ( (*job)->getStatus() == 2 ) {
      (*job)->setStatus(1);
      (*job)->run();
    }
  }
}
void Scheduler::start() {
  // Order matters, since the server blocks waiting for requests
  for (auto it = resPool->begin(); it != resPool->end(); it++)
    (*it)->start();
  //Fire up the Scheduling and dispatch threads.
  schedulerThread = new boost::thread(&Scheduler::schedLoop, this);
  dispatcherThread = new boost::thread(&Scheduler::dispatcherLoop, this);
  run();
}

void Scheduler::stop() {
  for (auto it = resPool->begin(); it != resPool->end(); it++)
    (*it)->start();
}
