#include <Scheduler.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>
#define MODE_MANAGED 4
using namespace std;




int Scheduler::getJobStatus(int jobID) {
  //check runQ
/*  for (auto it = runQ->begin(); it != runQ->end(); it++) {
    if ((std::get<0>(**it))->getId() == jobID) {
      return (std::get<0>(**it))->getStatus();
    }
  }*/
  cout << "Scheduler::getJobStatus: Not implemented\n";
  return -1;
  //check readyQ
  //check finishedQ
}

//TODO[mtottenh] Finish implementing the rest of the scheduler class
void Scheduler::defaultHandler(msg_t& request, msg_t& response, int responseSize) {
  /*
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
  response.print();*/

  cout << "Default Handler: Not implemented\n";
}




JobPtr Scheduler::estimateFinishTime(JobPtr j) {
  //TODO[mtottenh]:Add code to estimate a job finish time
  std::cout << "Scheduler::estimateFinishTime() : Not implemented\n";
  return j;
}


/*
 * TODO[mtottenh]: Check implementations of getDispatchTime/IssueTime with timevals
 * in the minites/seconds range not just random floats.. Also add Q locks
 */

int Scheduler::numLateJobs() {
  int sum = 0;
  auto it = finishedQ->begin();
  for(;it != finishedQ->end(); it++) {
    sum = (std::get<0>(**it))->getDispatchTime() - (std::get<0>(**it))->getIssueTime() > 1 ? sum+1 : sum;
  }
  return sum;
}

void Scheduler::updateState() {

}

void Scheduler::dumpInfo() {

}
void Scheduler::notifyClientsOfResults() {
  std::cout << "NotifyClients Of Results: Needs implementing\n";
}
void Scheduler::printQInfo(const char*, JobQueuePtr, bool) {

}
//TODO[mtottenh]: Add this to the header file. This could be
// dangerous.. it seems to remove all the resources in the runQ and return them to the pool.
// use with extreme caution
void Scheduler::reclaimResources() {
  cout << "Scheduler::reclaimResources(): Not Implemented\n";
  /*
  auto j = runQ->begin();
  for (;j != runQ->end(); j++) {
    auto a = (std::get<0>(**j))->getAllocdRes()->begin();
    for (;a != (std::get<0>(**j))->getAllocdRes()->end(); a++) {
      resPool->push_back(   move(*a)   );
    }
  }
*/
}



//TODO[mtottenh]: Collapse the below back into 1 function. needless code duplication here.
/*void Scheduler::schedule(int choice) {
  if (resPool->size() > 0) {
    this->algVec[choice](*this).serviceAllocations(*this);
  }
}*/
Allocations* Scheduler::schedule(int choice, bool flag) {
  flag = false;
  Allocations *a;
  if (resPool->size() > 0) {
    a = this->algVec[choice](*this);
  }
 return a;
}

std::deque<Resource> Scheduler::allocate(Job &j, int max_res, int min_res) {
  std::deque<Resource> allocatedResources;

  if (resPool->size() >= min_res) {
    while (max_res-- > 0 && resPool->size() > 0) {
      allocatedResources.push_back(*resPool->back());

     // j.allocate( move(resPool->back()) );
      resPool->pop_back();
    }
    std::cout << "Allocated " << allocatedResources.size()
              << " Resources to " << j << "\n";
  }
  return allocatedResources;
}

//TODO[mtottenh]:Check this functions as intended. I'm not sure about getAllocdRes()->***/
JobPtr Scheduler::deallocate(JobPtr j) {
  std::cout << "De-allocating (NOT IMPLEMENTED) " << *j << "\n";
/*            << "{" << j->noAllocdRes() << "}"
            << "|" << resPool->size() << "|";
  auto r = j->getAllocdRes()->begin();
  for (; r != j->getAllocdRes()->end(); r++) {
    resPool->push_back( move(*r ));
  }
  j->getAllocdRes()->clear();
  std::cout << "(" << resPool->size() << ")\n";*/
  return j;
}


//TODO[mtottenh]: Remove this
msg_t Scheduler::getJobResponse(int jobID) {
/*  msg_t response;
  char buffer[1024];
  bzero(buffer, 1024);
  for (auto job = runQ->begin(); job != runQ->end(); job++) {
    if (std::get<0>(**job)->getId() == jobID) {
      std::get<0>(**job)->getResponse(buffer,1024);
    }
  }

  msg_t* rsp = (msg_t*)buffer;
  response.msgId = rsp->msgId;
  response.dataSize = rsp->dataSize;
  response.paramsSize = 0;
  memcpy(response.data, rsp->data, rsp->dataBytes());
  return response;*/
  std::cout << "Scheduler::getJobResponse: Not Implmented\n";
  msg_t rsp;
  return rsp;
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
      break;
  }
}


void Scheduler::schedLoop() {
  QStatus.setReadyQStatus(false);
  QStatus.setFinishedQStatus(false);
  while (true) {
    //wait on readyQ or FinishedQ condvar
    boost::unique_lock<boost::mutex> lock(qMutex);
    //If we have no results or nothing to try and schedule wait
    while ( !QStatus.getReadyQStatus() && !QStatus.getFinishedQStatus() ) {
      QCondVar.wait(lock);
      std::cout << "Scheduler Thread woke up\n";
    } 
    //A job was deposited in the readyQ
    if (QStatus.getReadyQStatus() == true) {
      std::cout << "Event happened: readyQ\n";
      std::cout << "Scheduling\n";
      auto a = schedule(MODE_MANAGED,true);  //TODO[mtottenh]: modify schedule to lock all Q's for now.
      lock.unlock();
      /* The result of scheduling will be
       * either a potentiall allocation of resources 
       * to Jobs.
       * OR 
       * Some invalid value to say that there
       * were no free resources.
       */
       if (a == nullptr) {
         /* No free resources.. Just block for
          * some more time :) 
          */
          std::cout << "Could not allocate any resources..\n";
       } else {
          size_t numJobsScheduled = a->noJobs();
          if ( numJobsScheduled > 0) {
            std::cout << "Allocated Resources\n";
            QStatus.setRunQStatus(true);
            a->serviceAllocations(*this);
            std::cout << "ReadyQ now contains [" << readyQ->size() << "] Jobs\n";
            if (readyQ->size() == 0)
              QStatus.setReadyQStatus(false);
            delete a;
  
            QCondVar.notify_all();
          } else {
            std::cout << "Could not allocate any resources...\n";
          }
       }
      } 

    //A result was returned on the finishedQ -- This could be delegated to 
    // a different thread
    if (QStatus.getFinishedQStatus() == true) {
      //communicate to main thread that jobs in the finished Q are done
      notifyClientsOfResults();
        //std::get<1>(*j).setFinished(true);
        //std::get<2>(*j).notify_all();
      QStatus.setFinishedQStatus(false);
    }
  }
}

void Scheduler::dispatcherLoop() {
  while (true) {
    //wait on readyQ or FinishedQ condvar
    boost::unique_lock<boost::mutex> lock(qMutex);
    //TODO[mtottenh]: change finishedQStatus to somethign that indicates a dfe
    //has a result. Might not be able to have this as a condvar wait
    //situation....
    while ( !QStatus.getRunQStatus() && !QStatus.getFinishedQStatus() ) {
      QCondVar.wait(lock);
      std::cout << "Dispatcher thread woke up\n";
    } 

  }
}

msg_t*  Scheduler::concurrentHandler( msg_t &request, msg_t &response, int sizeBytes) {
  //creat (job,condvar) pair
  //
  boost::condition_variable jCondVar;
  boost::mutex jobMutex;
  boost::unique_lock<boost::mutex> lock(jobMutex);
  struct JobInfo jInfo;
  jInfo.setFinished(false);
  std::cout << "In ConcurrentHandler - Recieved Job\n" << std::endl;
  auto t = std::make_tuple( new Job(request,getNextId()),std::ref(jInfo),std::ref(jCondVar));
//   std::get<0>(t) = // std::get<0>(t) = std::unique_ptr<Job> ( new Job(request,getNextId()));
  enqueue(readyQ, &t, readyQMtx,"readyQ");
  

  while (!jInfo.isFinished()) {
    jCondVar.wait(lock);
  }
  std::cout << "Job finished\n" << std::endl;
  
  //dequeue(finishedQ,(job,condvar));
  return nullptr;
}




void Scheduler::start() {
  // Order matters, since the server blocks waiting for requests
//  for (auto it = resPool->begin(); it != resPool->end(); it++)
//    (*it)->start();
  //Fire up the Scheduling and dispatch threads.
  schedulerThread = new boost::thread(&Scheduler::schedLoop, this);
  dispatcherThread = new boost::thread(&Scheduler::dispatcherLoop, this);
  run();
}

void Scheduler::stop() {
//  for (auto it = resPool->begin(); it != resPool->end(); it++)
//    (*it)->start();
}
