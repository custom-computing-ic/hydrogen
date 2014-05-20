#include <Scheduler.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>
#define MODE_MANAGED 4
using namespace std;





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

  cout << "Default Handler: Not implemented\n" << responseSize;
  request.print();
  response.print();
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
  JobQueue::iterator it = finishedQ->begin();
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
Allocations* Scheduler::schedule(size_t choice, bool flag) {
  flag = false;
  Allocations *a = nullptr;
  if (resPool->size() > 0) {
    a = this->algVec[choice](*this);
  }
 return a;
}

std::deque<Resource> Scheduler::allocate(Job &j, size_t max_res, size_t min_res) {
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
  std::cout << "Scheduler::getJobResponse: Not Implmented" << jobID << "\n";
  msg_t rsp;
  return rsp;
}

/* Server Handling */
msg_t* Scheduler::handle_request(msg_t* request) {
  cout << "Scheduler recieved request msgID[" <<  request->msgId << "]" << endl;
  request->print();
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
      unsigned long sizeBytes = sizeof(msg_t) + sizeof(int) * request->dataSize;
      response = (msg_t*)calloc(sizeBytes, 1);
      return  concurrentHandler(*request, *response, sizeBytes);
  }
}


void Scheduler::schedLoop() {
  try {
    while (true) {
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
        Allocations* a = schedule(MODE_MANAGED,true);  //TODO[mtottenh]: modify schedule to lock all Q's for now.
        lock.unlock();
        if (a == nullptr) {
          /* No free resources.. Just block for some more time :)   */
          std::cout << "Could not allocate any resources..\n";
        } else {
          /* managed to get some kind of schedule. */
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

      //A result was returned on the finishedQ -- This could be delegated to a different thread
      if (QStatus.getFinishedQStatus() == true) {
        notifyClientsOfResults();
        QStatus.setFinishedQStatus(false);
      }
    } 
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
    this->stop();
  }
}
void Scheduler::runJobs() {
    JobResPairQ::iterator it = runQ->begin();
    for (;it != runQ->end(); it++) {
      //for each job in the runQ check if it is started.
      //if not, start it running
      JobTuplePtr jobTuplePtr = std::get<0>(*it);
      if (! std::get<1>(*jobTuplePtr).isStarted()){
        runJob(*it); // spawn this into a separate thread?
      }
    }
}
void Scheduler::runJob(JobResPair& j) {
  ResourceList ResourceList = std::get<1>(j);
  JobTuplePtr jobTuplePtr = std::get<0>(j);
  JobPtr jobPtr = std::get<0>(*jobTuplePtr);
 //TODO[mtottenh]: How do we invoke a job on more than one DFE? :O 
  Resource r = ResourceList.front();
  const string& name = r.getName().c_str();
  int portNumber = r.getPort();
  Client c(portNumber,name);
  c.start();
  msg_t& req = jobPtr->getReq();
  req.print();

  c.send(&req);
  char* buff = new char[1024];
  memset(buff,0,1024);
  int sizeBytes = sizeof(msg_t) + sizeof(int) * req.dataSize;
  msg_t* rsp = (msg_t*)buff;
  do {
    c.read(buff,sizeBytes);
    rsp->print();
  }  while ( rsp->msgId != MSG_RESULT);
  c.stop();
//  c.read(buff,sizeBytes);
  std::cout << "client::read() finished\n";

//  msg_t* rsp = (msg_t*)buff;
  rsp->print();
  jobPtr->setRsp(rsp);
  std::get<1>(*jobTuplePtr).setFinished(true);
  std::get<2>(*jobTuplePtr).notify_all();
}
void Scheduler::dispatcherLoop() {
  try {
    while (true) {
      //wait on readyQ or FinishedQ condvar
      boost::unique_lock<boost::mutex> lock(qMutex);
      //TODO[mtottenh]: change finishedQStatus to somethign that indicates a dfe
      //has a result. Might not be able to have this as a condvar wait
      //situation....
      while ( !QStatus.getRunQStatus() ) {
        QCondVar.wait(lock);
        std::cout << "Dispatcher thread woke up\n";
      }
      if (QStatus.getRunQStatus()) {
        std::cout << "Event on Run Q, itterating overQ and starting any"
                  << "Jobs \n";
        runJobs(); 
      } 
    }
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
    this->stop();
  }
}

msg_t*  Scheduler::concurrentHandler( msg_t &request, msg_t &response, unsigned long sizeBytes) {
  //creat (job,condvar) pair
  //
  boost::condition_variable jCondVar;
  boost::mutex jobMutex;
  boost::unique_lock<boost::mutex> lock(jobMutex);
  struct JobInfo jInfo;
  jInfo.setFinished(false);

  std::cout << "In ConcurrentHandler - Recieved Job: DataSize" 
            << sizeBytes << "\n" << std::endl;
  request.print();
  JobTuple t = std::make_tuple( new Job(request,getNextId()),std::ref(jInfo),std::ref(jCondVar));
//   std::get<0>(t) = // std::get<0>(t) = std::unique_ptr<Job> ( new Job(request,getNextId()));
  enqueue(readyQ, &t, readyQMtx,"readyQ");
  

  while (!jInfo.isFinished()) {
    jCondVar.wait(lock);
  }
  std::cout << "Job finished\n" << std::endl;
  
  //dequeue(finishedQ,(job,condvar));
  return std::get<0>(t)->getRsp();
}




void Scheduler::start() {
  // Order matters, since the server blocks waiting for requests
//  for (auto it = resPool->begin(); it != resPool->end(); it++)
//    (*it)->start();
  //Fire up the Scheduling and dispatch threads.
  QStatus.setReadyQStatus(false);
  QStatus.setRunQStatus(false);
  QStatus.setFinishedQStatus(false);

  schedulerThread = new boost::thread(&Scheduler::schedLoop, this);
  dispatcherThread = new boost::thread(&Scheduler::dispatcherLoop, this);
  run();
//  schedulerThread->join();
//  dispatcherThread->join();
}

