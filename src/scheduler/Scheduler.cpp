#include <Scheduler.hpp>
#include <iostream>
#include <boost/make_shared.hpp>
#include <cstring>
#include <boost/chrono.hpp>
#include <unistd.h>

#include <Logging.hpp>
#include <sstream>

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

// Removes resources allocated to a job from the res pool....
bool resPtrEQ(const Resource &lhs, const boost::shared_ptr<Resource> &rhs) {
  return lhs.getId() == rhs->getId();
}

void Scheduler::claimResources(JobResPairPtr elem) {
  boost::lock_guard<boost::mutex> lk(resPoolMtx);
  LOG(debug) << "resPool[" << resPool->size() << "]\t";
  ResourceListPtr r = std::get<1>(*elem);
  for (auto rit = r->begin(); rit != r->end(); rit++) {
    auto remove =
        std::find_if(resPool->begin(), resPool->end(),
                     std::bind(resPtrEQ, *rit, std::placeholders::_1));
    resPool->erase(remove);
  }
  LOG(debug) << "resPool[" << resPool->size() << "]";
}

void Scheduler::returnResources(ResourceListPtr res) {
  ResourceList::iterator r = res->begin();
  for (; r != res->end(); r++) {
   addResource(*r);
  }
}

bc::duration<double> Scheduler::estimateExecutionTime(JobPtr j) {
  // TODO[mtottenh]:Add code to estimate a job finish time
  double milliseconds = (double)j->getReq()->predictedTimeMillis;
  bc::duration<double, boost::ratio<1, 1000> > milli_sec(milliseconds);
  auto micro_sec = bc::duration_cast<bc::microseconds>(milli_sec);
  LOG(debug) << milli_sec.count() << "ms"
	     << ", " << micro_sec.count() << "us";
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
  for (; it != finishedQ->end(); it++) {
    bc::system_clock::time_point disp_t =
        (std::get<0>(**it))->getDispatchTime();
    bc::system_clock::time_point issue_t = (std::get<0>(**it))->getIssueTime();
    sum = (disp_t - issue_t) > bc::seconds(1) ? sum + 1 : sum;
  }
  return sum;
}

void Scheduler::updateState() {}

void Scheduler::dumpInfo() {}

void Scheduler::notifyClientsOfResults() {
  boost::unique_lock<boost::mutex> lk(finishedQMtx);
  auto j = finishedQ->front();
  finishedQ->pop_front();
  lk.unlock();
  updateStatistics(std::get<0>(*j));

  std::get<1>(*j)->setFinished(true);
  std::get<2>(*j)->notify_all();
}

Allocations *Scheduler::schedule(size_t choice, bool flag) {
  flag = false;
  Allocations *a = nullptr;
  if (resPool->size() <= 0) {
    LOG(debug) << "No free resources.";
  }
  if (readyQ->size() <= 0) {
    LOG(debug) << "No waiting jobs.";
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

    LOGF(debug, "Allocated %1% resources to %2% ") % allocatedResources.size() % j;
  }
  return allocatedResources;
}

// TODO[mtottenh]:Check this functions as intended.
// I'm not sure about getAllocdRes()->***/
JobPtr Scheduler::deallocate(JobPtr j) {
  LOG(error) << "NOT IMPLEMENTED";
  return j;
}

/* Server Handling */
msg_t *Scheduler::handle_request(msg_t *request) {
  LOGF(debug, "Scheduler recieved request msgID[%1%]") % request->msgId;
  msg_t *response;
  unsigned long sizeBytes = 0;
  switch (request->msgId) {
  case MSG_TERM:
    stop();
    return (msg_t *)NULL;
  case MSG_OPTION_PRICE:
    sizeBytes = sizeof(msg_t) + request->expDataSizeBytes;
    response = (msg_t *)calloc(sizeBytes, 1);
    LOG(debug) << "Handling MSG_OPTION_PRICE request";
    concurrentHandler(*request, *response, sizeBytes);
    return response;
  case MSG_MOVING_AVG:
    sizeBytes = sizeof(msg_t) + request->expDataSizeBytes;
    response = (msg_t *)calloc(sizeBytes, 1);
    concurrentHandler(*request, *response, sizeBytes);
    response->avg_wt =
        (int)bc::duration_cast<bc::milliseconds>(meanWaitTime).count();
    return response;
  default:
    LOGF(error, "Unsuported msg_id %1%") % request->msgId;
  case MSG_DONE:
    return msg_ack();
  }
}

void Scheduler::schedLoop() {
  try {
    while (true) {
      boost::unique_lock<boost::mutex> lock(qMutex);
      // If we have no results or nothing to try and schedule wait
      while (!QStatus.getReadyQStatus()) {
        QCondVar.wait(lock);
        //  LOG(debug) << "Scheduler Thread woke up";
      }
      lock.unlock();

      // A job was deposited in the readyQ
      if (QStatus.getReadyQStatus() == true) {
        QStatus.setReadyQStatus(false);
        auto start = bc::system_clock::now();
        //        LOG(debug) << "(DEBUG): Event on readyQ";
        boost::unique_lock<boost::mutex> rqLk(readyQMtx);
        Allocations *a = schedule(MODE_MANAGED, true);
        rqLk.unlock();
        if (a == nullptr) {
          /* No free resources.. Just block for some more time :)   */
          //          LOG(debug) << "(DEBUG): Allocations returned null";
          boost::this_thread::yield();

          QStatus.setRunQStatus(true);
          QCondVar.notify_all();
        } else {
	  elasticityManager.updateResourcePool(*this, *a);

          /* managed to get some kind of schedule. */
          size_t numJobsScheduled = a->noJobs();
          if (numJobsScheduled > 0) {
            a->serviceAllocations(*this);
            QStatus.setRunQStatus(true);
            auto end = bc::system_clock::now();
            auto duration = bc::duration_cast<bc::microseconds>(end - start);

            LOGF(debug, "Scheduling took: %1%") % duration.count();
            totalSchedTime += duration;
            numSchedules++;
            QCondVar.notify_all();
          } else {
            boost::this_thread::yield();
          }
          delete a;

          LOG(debug) << "Managed Mode scheduled " << numJobsScheduled;
          totalJobsScheduled += numJobsScheduled;
          if (numJobsScheduled > 1)
            LOG(debug) << " Jobs";
          else
            LOG(debug) << " Job";
          size_t waitingJobs = readyQ->size();
          LOG(debug) << "\t(" << waitingJobs;
          if (waitingJobs > 1 || waitingJobs == 0)
            LOG(debug) << " Jobs left)";
          else
            LOG(debug) << " Job left)";
        }
        QStatus.setReadyQStatus(readyQ->size() > 0); // hangs here....
      }
    }
  }
  catch (boost::thread_interrupted &) {
    LOG(debug) << "\t* Scheduling thread recieved interrupt";
    return;
  }
  catch (std::exception &e) {
    LOG(debug) << "(ERROR): Scheduling thread - " << e.what();
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
    if (!jobInfo->isStarted()) {
      LOG(debug) << "Start Job";
      jobInfo->setStarted(true);
      boost::thread jobThread(
          boost::bind(&Scheduler::runJob, this, job_pair_ptr));
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
  std::tie(jobTuplePtr, resourceList) = *j;
  JobPtr jobPtr = std::get<0>(*jobTuplePtr);
  LOG(debug) << "- Scheduler::runJob(" << *jobPtr << ")";
  LOG(debug) << "- Allocated[" << resourceList->size() << "]";
  char packed_rids = 0x0;

  std::stringstream ss;
  for (auto &r : *resourceList) {
    ss << r.getId() << ", ";
    switch (r.getId()) {

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
  LOG(debug) << "\t* Rids: { " << ss.str()  <<  "}";
  Resource r = resourceList->front();
  const string &name = r.getName().c_str();
  int portNumber = r.getPort();
  msg_t *req = jobPtr->getReq();

  LOG(debug) << "\t* Opening connection to: " << name << ":"
            << portNumber << "";

  // TODO[mtottenh]: How do we invoke a job on more than one DFE? :O
  // TODO[mtottenh]: Quick hack for now, need to make this scaleable
  Client c(portNumber, name);
  auto start = bc::system_clock::now();
  c.start();

  boost::this_thread::interruption_point();
#ifdef DEBUG
  req->print();
#endif
  if (req != NULL) {
    //     LOG(debug) << "\t* Scheduler::runJob() - Sending Request";
    req->rids = packed_rids;
    c.send(req);
  }

  int sizeBytes = sizeof(msg_t) + req->expDataSizeBytes;
  char *buff = (char *)calloc(sizeBytes, 1);

  if (buff == NULL) {
    LOG(debug) << "\tUnable to allocate result buffer";
    c.stop();
    returnResources(resourceList);
    resourceList = nullptr;
  }

  msg_t *rsp = (msg_t *)buff;

  do {
    LOG(debug) << "\t Reading reply, size: " << sizeBytes;
    c.read(buff, sizeBytes);
#ifdef DEBUG
    rsp->print();
#endif
  } while (rsp->msgId != MSG_RESULT);
  LOG(debug) << "\t* Got Result";
  c.stop();
  auto end = bc::system_clock::now();
  jobPtr->setMeasuredExecutionTime(end - start);
  jobPtr->setActualExecutionTime(bc::milliseconds(rsp->predictedTimeMillis));
#ifdef DEBUG
  rsp->print();
#endif
  //    jobPtr->copyRsp(buff,sizeBytes);
  jobPtr->setRsp((msg_t *)buff);
  returnResources(resourceList);
  resourceList = nullptr;
  enqueue(finishedQ, jobTuplePtr, finishedQMtx, "finishedQ");
  removeJobFromRunQ(jobPtr->getId());
}

void Scheduler::removeJobFromRunQ(int jid) {
  boost::lock_guard<boost::mutex> lk(runQMtx);
  auto it = std::find_if(runQ->begin(), runQ->end(),
                         std::bind(&idEq, jid, std::placeholders::_1));
  if (it != runQ->end())
    runQ->erase(it);
  QCondVar.notify_all();
}

void Scheduler::dispatcherLoop() {
  try {
    while (true) {
      // wait on readyQ or FinishedQ condvar
      boost::unique_lock<boost::mutex> lock(qMutex);
      // TODO[mtottenh]: change finishedQStatus to somethign that indicates a
      // dfe
      // has a result. Might not be able to have this as a condvar wait
      // situation....
      while (!QStatus.getRunQStatus()) {
        QCondVar.wait(lock);
        //        LOG(debug) << "(DEBUG): Dispatcher thread woke up";
      }
      lock.unlock();
      if (QStatus.getRunQStatus()) {
        //        LOG(debug) << "(DEBUG): Event on Run Q";
        while (runQ->size() > 0) {
          runJobs();
        }
        //        if (runQ->size() == 0)
        QStatus.setRunQStatus(false);
      }
    }
  }
  catch (boost::thread_interrupted &) {
    LOG(debug) << "\t* Dispatcher thread recieved interrupt";
    return;
  }
}

msg_t *Scheduler::concurrentHandler(msg_t &request, msg_t &response,
                                    unsigned long sizeBytes) {
  // creat (job,condvar) pair
  //
  auto jCondVar = CondVarPtr(new boost::condition_variable);
  boost::mutex jobMutex;
  boost::unique_lock<boost::mutex> lock(jobMutex);
  //  struct JobInfo jInfo;
  auto jInfo = JobInfoPtr(new JobInfo());
  jInfo->setFinished(false);
  jInfo->setStarted(false);
  //  LOGF(debug, " New Connection tid = %1%") % boost::this_thread::get_id();

#ifdef DEBUG
  request.print();
#endif
  JobPtr nJob = JobPtr(new Job(&request, getNextId()));
  JobTuple t = std::make_tuple(nJob, jInfo, jCondVar);
  enqueue(readyQ, std::make_shared<JobTuple>(t), readyQMtx, "readyQ");

  try {
    while (!jInfo->isFinished()) {
      jCondVar->wait(lock);
    }
    lock.unlock();
    LOG(debug) << *nJob << " finished";
    msg_t *rsp = nJob->getRsp();
    if (rsp == NULL) {
      // ERROR
    } else {
      response.msgId = rsp->msgId;
      response.dataSize = rsp->dataSize;
      response.paramsSize = 0;
      response.totalBytes = rsp->totalBytes;
      response.dataBytes = rsp->dataBytes;
      memcpy(&response.data, rsp->data, rsp->dataBytes);
      free(rsp);
    }
  }
  catch (boost::thread_interrupted &) {
    LOG(debug) << "Concurrent handler interrupted";
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
      while (!QStatus.getFinishedQStatus()) {
        QCondVar.wait(lock);
      }
      lock.unlock();
      if (QStatus.getFinishedQStatus() == true) {
        QStatus.setReadyQStatus(true);
        while (finishedQ->size() > 0) {
          notifyClientsOfResults();
        }
        QStatus.setFinishedQStatus(false);
      }
    }
  }
  catch (boost::thread_interrupted &) {
    LOG(debug) << "\t* FinishedQ thread recieved interrupt";
    return;
  }
  catch (std::exception &e) {
    LOG(debug) << "(ERROR): FinishedQ thread - " << e.what();
  }
}

void Scheduler::start() {
  // Fire up the Scheduling and dispatch threads.
  QStatus.setReadyQStatus(false);
  QStatus.setRunQStatus(false);
  QStatus.setFinishedQStatus(false);

  schedulerThread = new boost::thread(&Scheduler::schedLoop, this);
  dispatcherThread = new boost::thread(&Scheduler::dispatcherLoop, this);
  finishedQThread = new boost::thread(&Scheduler::finishedLoop, this);
  run();
}
