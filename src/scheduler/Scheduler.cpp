#include <Scheduler.hpp>
#include <iostream>
#include <boost/make_shared.hpp>
#include <cstring>
#include <boost/chrono.hpp>
#include <unistd.h>
#define REF_GET(N,Tuple) \
        [] (Tuple& t) { return std::get<N>(t);};

#define FCFS_MIN 0
#define FCFS_MAX 1
#define FCFS_AMAP 2
#define SJTF 3
#define PRIORITY 4
#define MODE_MANAGED 5
#define RID0 0x0
#define RID1 0x1
#define RID2 0x2
#define RID3 0x4
#define RID4 0x8
namespace bc = boost::chrono;

using namespace std;
//Removes resources allocated to a job from the res pool....
bool resPtrEQ(const Resource& lhs, const boost::shared_ptr<Resource>& rhs) {
  return lhs.getId() == rhs->getId();
}
void Scheduler::claimResources(JobResPairPtr elem) {
  boost::lock_guard<boost::mutex> lk(resPoolMtx);
  using namespace std::placeholders;
  std::cout << "(DEBUG):\t- Scheduler::claimResources()\n";
//  std::cout << "resPool[" << resPool->size() << "]\t";

  ResourceListPtr r = std::get<1>(*elem);
  for (auto rit = r->begin(); rit != r->end(); rit++) {
    auto remove = std::find_if(resPool->begin(), resPool->end(),std::bind(resPtrEQ,*rit,std::placeholders::_1));
    resPool->erase(remove);
  }
//  std::cout << "resPool[" << resPool->size() << "]\n";
}

void Scheduler::returnResources(ResourceListPtr res) {
  ResourceList::iterator r = res->begin();
  for (; r != res->end(); r++) {
    addResource(*r);
  }
}

//TODO[mtottenh] Finish implementing the rest of the scheduler class
void Scheduler::defaultHandler(msg_t& request,
                               msg_t& response,
                               int responseSize) {
  cout << "(ERROR): Default Handler: Not implemented\n" << responseSize;
#ifdef DEBUG
  request.print();
  response.print();
#endif
}

bc::duration<double> Scheduler::estimateExecutionTime(JobPtr j) {
  //TODO[mtottenh]:Add code to estimate a job finish time
  std::cout << "(DEBUG): Scheduler::estimateExecutionTime()\n";
  double milliseconds = (double) j->getReq()->predicted_time;
  bc::duration<double,boost::ratio<1,1000>> milli_sec(milliseconds);
  auto micro_sec = bc::duration_cast<bc::microseconds>(milli_sec);
  std::cout << "(DEBUG):\t- " << milli_sec.count() << "ms\n";
  std::cout << "(DEBUG):\t- " << micro_sec.count() << "us\n";
  return milli_sec;
}


/*
 * TODO[mtottenh]: Check implementations of getDispatchTime/IssueTime
 *  with timevals in the minites/seconds range not just random
 *  floats.. Also add Q locks
 */

int Scheduler::numLateJobs() {
  boost::lock_guard<boost::mutex> lk(finishedQMtx);
  int sum = 0;
  JobQueue::iterator it = finishedQ->begin();
  for(;it != finishedQ->end(); it++) {
    bc::system_clock::time_point disp_t = (std::get<0>(**it))->getDispatchTime();
    bc::system_clock::time_point issue_t = (std::get<0>(**it))->getIssueTime();
    sum = (disp_t - issue_t)  > bc::seconds(1) ? sum+1 : sum;
  }
  return sum;
}

void Scheduler::updateState() {

}

void Scheduler::dumpInfo() {

}

void Scheduler::notifyClientsOfResults() {
//  std::cout << "NotifyClients Of Results: Needs implementing\n";
  boost::unique_lock<boost::mutex> lk(finishedQMtx);
  auto j = finishedQ->front();
  finishedQ->pop_front();
  lk.unlock();
  updateStatistics(std::get<0>(*j));

  std::get<1>(*j)->setFinished(true);
  std::get<2>(*j)->notify_all();
}

void Scheduler::printQInfo(const char*, JobQueuePtr, bool) {

}
//TODO[mtottenh]: Add this to the header file. This could be
// dangerous.. it seems to remove all the resources in the runQ and
// return them to the pool... use with extreme caution
void Scheduler::reclaimResources() {
  cout << "Scheduler::reclaimResources(): Not Implemented\n";
}

//TODO[mtottenh]: Collapse the below back into 1 function.
// needless code duplication here.
Allocations* Scheduler::schedule(size_t choice, bool flag) {

  flag = false;
  Allocations *a = nullptr;
  if (resPool->size() <= 0) {
//    std::cout << "(DEBUG): No free resources.\n";

  }
  if (readyQ->size() <= 0) {
//    std::cout << "(DEBUG): No waiting jobs.\n";
  }

  if (resPool->size() > 0 && readyQ->size() > 0) {
    a = this->algVec[choice](*this);
  }
 return a;
}

ResourceList Scheduler::allocate(Job &j, size_t max_res, size_t min_res) {
  ResourceList allocatedResources;

  if (resPool->size() >= min_res) {
    while (max_res-- > 0 && resPool->size() > 0) {
      allocatedResources.push_back(*resPool->back());
      resPool->pop_back();
    }
#ifdef DEBUG
    std::cout << "Allocated " << allocatedResources.size()
              << " Resources to " << j << "\n";
#endif
  }
  return allocatedResources;
}

//TODO[mtottenh]:Check this functions as intended.
//I'm not sure about getAllocdRes()->***/
JobPtr Scheduler::deallocate(JobPtr j) {
  std::cout << "(ERROR): Scheduler::deallocate (NOT IMPLEMENTED) \n";
  return j;
}


//TODO[mtottenh]: Remove this
msg_t Scheduler::getJobResponse(int jobID) {
  std::cout << "(ERROR): Scheduler::getJobResponse (NOT IMPLEMENTED)\n";
  msg_t rsp;
  return rsp;
}

/* Server Handling */
msg_t* Scheduler::handle_request(msg_t* request) {
#ifdef DEBUG
  cout << "Scheduler recieved request msgID[" <<  request->msgId << "]" << endl;
  request->print();
#endif
  //TODO: Lookup requestID/Implementation ID in a map and return error if not
  //found
  msg_t* response;
  unsigned long sizeBytes = 0;
  switch(request->msgId) {
    case MSG_TERM:
        stop();
        return (msg_t*)NULL;
    case MSG_OPTION_PRICE:
      // XXX TODO[paul-g]: need to actually determine data size here
      sizeBytes = sizeof(msg_t) + request->expDataSizeBytes;
      response = (msg_t*)calloc(sizeBytes, 1);
      cout << "(DEBUG): Scheduler:: Handling msg_option request" << endl;
      concurrentHandler(*request, *response, sizeBytes);
#ifdef DEBUG
      response->print();
      cout << "Returning from Scheduler::handle_request()\n";
#endif
      return response;
    case MSG_MOVING_AVG:
      // XXX TODO[paul-g]: need to actually determine data size here
      sizeBytes = sizeof(msg_t) + request->expDataSizeBytes;
      response = (msg_t*)calloc(sizeBytes, 1);
      concurrentHandler(*request, *response, sizeBytes);
      response->avg_wt = (int)bc::duration_cast<bc::milliseconds>(meanWaitTime).count();
#ifdef DEBUG
      response->print();
      cout << "Returning from Scheduler::handle_request()\n";
#endif

      return response;
    default:
      cerr << "Error! Unsuported msg_id " << request->msgId << endl;
    case MSG_DONE:
      return msg_ack();
  }
}


void Scheduler::schedLoop() {
  try {
    while (true) {
      boost::unique_lock<boost::mutex> lock(qMutex);
      //If we have no results or nothing to try and schedule wait
      while ( !QStatus.getReadyQStatus()) {
        QCondVar.wait(lock);
      //  std::cout << "Scheduler Thread woke up\n";
      }
      lock.unlock();

      //A job was deposited in the readyQ
      if (QStatus.getReadyQStatus() == true) {
        QStatus.setReadyQStatus(false);
        auto start = bc::system_clock::now();
//        std::cout << "(DEBUG): Event on readyQ\n";
        boost::unique_lock<boost::mutex> rqLk(readyQMtx);
        Allocations* a = schedule(MODE_MANAGED,true);
        rqLk.unlock();
        if (a == nullptr) {
          /* No free resources.. Just block for some more time :)   */
//          std::cout << "(DEBUG): Allocations returned null\n";
        boost::this_thread::yield();

        QStatus.setRunQStatus(true);
        QCondVar.notify_all();
        } else {
          /* managed to get some kind of schedule. */
          size_t numJobsScheduled = a->noJobs();
          if ( numJobsScheduled > 0) {
            a->serviceAllocations(*this);
            QStatus.setRunQStatus(true);
            auto end = bc::system_clock::now();
            auto duration = bc::duration_cast<bc::microseconds>(end - start);

            std::cout << "(DEBUG): Scheduling took: " << duration.count() << "us"
                      << std::endl;
            totalSchedTime += duration;
            numSchedules++;
            QCondVar.notify_all();
          } else {
            boost::this_thread::yield();
          }
          delete a;

          std::cout << "(DEBUG): Managed Mode scheduled " << numJobsScheduled;
          totalJobsScheduled += numJobsScheduled;
          if (numJobsScheduled > 1)
            std::cout <<" Jobs";
          else
            std::cout <<" Job";
          size_t waitingJobs = readyQ->size();
          std::cout << "\t(" << waitingJobs;
          if (waitingJobs > 1 || waitingJobs == 0)
            std::cout <<" Jobs left)\n";
          else
            std::cout <<" Job left)\n";

        }
        QStatus.setReadyQStatus(readyQ->size() > 0); // hangs here....
      }
    }
  }
  catch (boost::thread_interrupted &) {
    std::cout << "(DEBUG):\t\t* Scheduling thread recieved interrupt"  << std::endl;
    return;
  }
  catch (std::exception &e) {
    std::cout << "(ERROR): Scheduling thread - " << e.what() << std::endl;
    throw e;
  }
}
void Scheduler::runJobs() {

    boost::unique_lock<boost::mutex> lk(runQMtx);
    if (runQ->size() > 0) {
//    boost::thread_group jT;
      auto job_pair_ptr = runQ->front();
      runQ->pop_front();
      lk.unlock();
      JobTuplePtr jobTuplePtr = std::get<0>(*job_pair_ptr);
      auto jobInfo = std::get<1>(*jobTuplePtr);
      if (! jobInfo->isStarted()) {
       std::cout << "(DEBUG): Scheduler::runJobs()\n";
       jobInfo->setStarted(true);
       boost::thread jobThread(boost::bind(&Scheduler::runJob,this,job_pair_ptr));
       jobThread.detach();
//         jT.create_thread(boost::bind(&Scheduler::runJob,this,*it));
      }
      if (!jobInfo->isFinished()) {
       lk.lock();
       runQ->push_back(job_pair_ptr);
       lk.unlock();
      }
    } else {
     lk.unlock();
    }

  //  jT.join_all();
}
void Scheduler::runJob(JobResPairPtr j) {
    ResourceListPtr resourceList;
    JobTuplePtr jobTuplePtr;
    std::tie(jobTuplePtr,resourceList) = *j;
    JobPtr jobPtr = std::get<0>(*jobTuplePtr);
    std::cout << "(DEBUG):\t- Scheduler::runJob(" << *jobPtr  << ")\n";
    std::cout << "(DEBUG):\t- Allocated[" << resourceList->size() <<"]\n";
    char packed_rids = 0x0;
    std::cout << "(DEBUG):\t\t* Rids: { ";
    for (auto &r : *resourceList) {
      std::cout << r.getId() << ", ";
      switch(r.getId()) {

        case 1:
          packed_rids |= RID1;
          break;
        case 2:
          packed_rids |= RID2;
          break;
        case 3:
          packed_rids |= RID3;
          break;
        case 4:
          packed_rids |= RID4;
          break;
      }
    }
    std::cout << "}\n";
    Resource r = resourceList->front();
    const string& name = r.getName().c_str();
    int portNumber = r.getPort();
    msg_t* req = jobPtr->getReq();

    std::cout << "(DEBUG):\t\t* Opening connection to: "
              << name << ":" << portNumber << "\n";

   //TODO[mtottenh]: How do we invoke a job on more than one DFE? :O
   //TODO[mtottenh]: Quick hack for now, need to make this scaleable
    Client c(portNumber,name);
    auto start = bc::system_clock::now();
    c.start();


    boost::this_thread::interruption_point();
    #ifdef DEBUG
      req->print();
    #endif
    if (req != NULL) {
 //     std::cout << "(DEBUG):\t\t* Scheduler::runJob() - Sending Request\n";
      req->rids = packed_rids;
      c.send(req);
    }

    int sizeBytes = sizeof(msg_t) + req->expDataSizeBytes;
    char* buff = (char *)calloc(sizeBytes, 1);

    if (buff == NULL) {
      std::cout << "(ERROR):\tUnable to allocate result buffer\n";
      c.stop();
      returnResources(resourceList);
      resourceList = nullptr;
    }

    msg_t* rsp = (msg_t*)buff;

    do {
      std::cout << "(INFO):\t Reading reply, size: " << sizeBytes << endl;
      c.read(buff, sizeBytes);
      #ifdef DEBUG
        rsp->print();
      #endif
    }  while ( rsp->msgId != MSG_RESULT);
    std::cout << "(DEBUG):\t\t* Got Result\n";
    c.stop();
    auto end = bc::system_clock::now();
    jobPtr->setMeasuredExecutionTime(end-start);
    jobPtr->setActualExecutionTime(bc::milliseconds(rsp->predicted_time));
    #ifdef DEBUG
      rsp->print();
    #endif
//    jobPtr->copyRsp(buff,sizeBytes);
    jobPtr->setRsp((msg_t*)buff);
    returnResources(resourceList);
    resourceList = nullptr;
    enqueue(finishedQ,jobTuplePtr,finishedQMtx,"finishedQ");
    removeJobFromRunQ(jobPtr->getId());

}
void Scheduler::removeJobFromRunQ(int jid) {
  boost::lock_guard<boost::mutex> lk(runQMtx);
  auto it = std::find_if(runQ->begin(), runQ->end(),
                          std::bind(&idEq,jid,std::placeholders::_1));
  if (it != runQ->end())
    runQ->erase(it);
  QCondVar.notify_all();
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
//        std::cout << "(DEBUG): Dispatcher thread woke up\n";
      }
      lock.unlock();
      if (QStatus.getRunQStatus()) {
//        std::cout << "(DEBUG): Event on Run Q\n";
        while(runQ->size() > 0) {
          runJobs();
        }
//        if (runQ->size() == 0)
        QStatus.setRunQStatus(false);
      }
    }
  }
  catch (boost::thread_interrupted &) {
    std::cout << "(DEBUG):\t\t* Dispatcher thread recieved interrupt"  << std::endl;
    return;
  }
/*  catch (std::exception &e) {
    std::cout << "(ERROR): Dispatcher thread - " << e.what() << std::endl;
    throw e;
  }*/
}

msg_t*  Scheduler::concurrentHandler( msg_t &request,
                                      msg_t &response,
                                      unsigned long sizeBytes) {
  //creat (job,condvar) pair
  //
  auto jCondVar = CondVarPtr(new boost::condition_variable);
  boost::mutex jobMutex;
  boost::unique_lock<boost::mutex> lock(jobMutex);
//  struct JobInfo jInfo;
  auto jInfo = JobInfoPtr(new JobInfo());
  jInfo->setFinished(false);
  jInfo->setStarted(false);
  std::cout << "(DEBUG): Scheduler::concurrentHandler()\n";
  std::cout << "(DEBUG):\t- New Connection on Thread: "
            << boost::this_thread::get_id() << "\n";

#ifdef DEBUG
  request.print();
#endif
  JobPtr nJob = JobPtr(new Job(&request,getNextId()));
  JobTuple t = std::make_tuple( nJob, jInfo ,jCondVar);
  enqueue(readyQ, std::make_shared<JobTuple>(t), readyQMtx,"readyQ");

  try {
    while (!jInfo->isFinished()) {
      jCondVar->wait(lock);
    }
    lock.unlock();
    std::cout << "(DEBUG): " << *nJob << " finished\n" << std::endl;
    msg_t* rsp = nJob->getRsp();
    if (rsp == NULL) {
      //ERROR
    } else {
      response.msgId = rsp->msgId;
      response.dataSize = rsp->dataSize;
      response.paramsSize = 0;
      response.totalBytes = rsp->totalBytes;
      response.dataBytes = rsp->dataBytes;
      memcpy(&response.data, rsp->data, rsp->dataBytes);
      free(rsp);
    }
  } catch (boost::thread_interrupted &)  {
    std::cout << "(DEBUG): Concurrent handler interrupted" << std::endl;
    return &response;
  }
#ifdef DEBUG
  response.print();
#endif
  return &response;
}


void Scheduler::finishedLoop() {
  try {
    while (true) {
      boost::unique_lock<boost::mutex> lock(qMutex);
      while (!QStatus.getFinishedQStatus() ) {
        QCondVar.wait(lock);
      }
      lock.unlock();
      if (QStatus.getFinishedQStatus() == true) {
//        std::cout << "(DEBUG): Event on finishedQ\n";
        QStatus.setReadyQStatus(true);
        while (finishedQ->size() > 0) {
          notifyClientsOfResults();
        }
        QStatus.setFinishedQStatus(false);
      }
    }
  }
  catch (boost::thread_interrupted &) {
    std::cout << "(DEBUG):\t\t* FinishedQ thread recieved interrupt"  << std::endl;
    return;
  }
  catch (std::exception& e) {
    std::cout << "(ERROR): FinishedQ thread - " << e.what() << std::endl;
  }
}

void Scheduler::start() {
  //Fire up the Scheduling and dispatch threads.
  QStatus.setReadyQStatus(false);
  QStatus.setRunQStatus(false);
  QStatus.setFinishedQStatus(false);

  schedulerThread = new boost::thread(&Scheduler::schedLoop, this);
  dispatcherThread = new boost::thread(&Scheduler::dispatcherLoop, this);
  finishedQThread = new boost::thread(&Scheduler::finishedLoop, this);
  run();
}
