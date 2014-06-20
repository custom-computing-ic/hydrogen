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

int Scheduler::numLateJobs() {
  // TODO[paul-g] move iteration inside the queue (e.g. a for_each style call)
  int sum = 0;
  auto it = finishedQ->begin();
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
  const Resource& r = resourceList->front();
  const string &name = r.getName().c_str();
  int portNumber = r.getPort();
  msg_t *req = jobPtr->getReq();

  LOG(debug) << "\t* Opening connection to: " << name << ":"
             << portNumber << "";

  Client c(portNumber, name);
  auto start = bc::system_clock::now();
  c.start();

  boost::this_thread::interruption_point();
  if (req != NULL) {
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
  } while (rsp->msgId != MSG_RESULT);
  LOG(debug) << "\t* Got Result";
  c.stop();
  auto end = bc::system_clock::now();
  jobPtr->setMeasuredExecutionTime(end - start);
  jobPtr->setActualExecutionTime(bc::milliseconds(rsp->predictedTimeMillis));
  jobPtr->setRsp((msg_t *)buff);
  returnResources(resourceList);
  resourceList = nullptr;
  std::get<0>(*jobTuplePtr)->setFinishTime(bc::system_clock::now());
  finishedQ->push_front(jobTuplePtr);
  LOG(debug) << "Removing job from runQ";
  removeJobFromRunQ(jobPtr->getId());
}

void Scheduler::removeJobFromRunQ(int jid) {
  //  boost::lock_guard<boost::mutex> lk(runQMtx);
  LOG(debug) << "removing job from runQ";
  auto it = std::find_if(runQ->begin(), runQ->end(),
                         std::bind(&idEq, jid, std::placeholders::_1));
  if (it != runQ->end())
    runQ->erase(it);
}

void Scheduler::schedLoop() {
  try {
    while (true) {
      readyQ->wait_not_empty();
      LOG(debug) << "Got job in schedLoop";
      auto start = bc::system_clock::now();
      Allocations *a = nullptr;
      if (resPool->size() > 0) {
	readyQ->lock();
	a = this->algVec[MODE_MANAGED](*this);
	readyQ->unlock();
      } else {
        continue;
      }

      /* managed to get some kind of schedule. */
      size_t numJobsScheduled = a->noJobs();
      if (numJobsScheduled > 0) {
	elasticityManager.updateResourcePool(*this, *a);
        a->serviceAllocations(*this);
        auto end = bc::system_clock::now();
        auto duration = bc::duration_cast<bc::microseconds>(end - start);
        LOGF(debug, "Scheduling took: %1%") % duration.count();
        totalSchedTime += duration;
        numSchedules++;
      }
      delete a;
      LOGF(debug, "MMode scheduled %1% job(s)") % numJobsScheduled;
      totalJobsScheduled += numJobsScheduled;
      LOG(debug) << "\t(" << readyQ->size() << " Job(s) left)";
    }
  }
  catch (std::exception &e) {
    LOG(error) << "Scheduling thread - " << e.what();
  }
}

void Scheduler::dispatcherLoop() {
  try {
    while (true) {
      auto job_pair_ptr = runQ->wait_pop_front();
      LOG(debug) << "Dispatcher serving job...";
      JobTuplePtr jobTuplePtr = std::get<0>(*job_pair_ptr);
      auto jobInfo = std::get<1>(*jobTuplePtr);
      if (!jobInfo->isStarted()) {
	LOG(debug) << "Start job on separate thread";
	jobInfo->setStarted(true);
	boost::thread jobThread (
				boost::bind(&Scheduler::runJob, this, job_pair_ptr));
	jobThread.detach();
      }

    }
  }
  catch (boost::thread_interrupted &) {
    LOG(debug) << "\t* Dispatcher thread recieved interrupt";
    return;
  }
}

/** Loop for processing finished jobs which should run on a separate
    thread.*/
void Scheduler::finishedLoop() {
  try {
    while (true) {
      // TOOD [paul-g]: there is a race condition here, but I am not
      // sure what is deleting the jobs and causing a segfault
      auto j = finishedQ->wait_pop_front();
      updateStatistics(std::get<0>(*j));
      LOG(debug) << "Updated statistics";
      std::get<1>(*j)->setFinished(true);
      std::get<2>(*j)->notify_all();
    }
  }
  catch (std::exception &e) {
    LOG(debug) << "FinishedQ thread - " << e.what();
  }
}

msg_t *Scheduler::concurrentHandler(msg_t &request, msg_t &response,
                                    unsigned long sizeBytes) {
  auto jCondVar = CondVarPtr(new boost::condition_variable);
  boost::mutex jobMutex;

  boost::unique_lock<boost::mutex> lock(jobMutex);
  auto jInfo = JobInfoPtr(new JobInfo());
  jInfo->setFinished(false);
  jInfo->setStarted(false);

  JobPtr nJob = JobPtr(new Job(&request, getNextId()));
  JobTuple t = std::make_tuple(nJob, jInfo, jCondVar);

  JobTuplePtr jobPtr = std::make_shared<JobTuple>(t);
  std::get<0>(*jobPtr)->setIssueTime(bc::system_clock::now());
  readyQ->push_front(jobPtr);

  LOGF(debug, " New conn tid = %1%, Added job to readyQ %2%")
    % boost::this_thread::get_id() % *nJob;

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


void Scheduler::start() {
  schedulerThread = new boost::thread(&Scheduler::schedLoop, this);
  dispatcherThread = new boost::thread(&Scheduler::dispatcherLoop, this);
  finishedQThread = new boost::thread(&Scheduler::finishedLoop, this);
  run();
}

JobResPairPtr Scheduler::removeJobFromReadyQ(const JobResPairPtr &j) {
  if (readyQ->size() > 0) {
    auto elem = std::find_if(
			     readyQ->begin(), readyQ->end(),
			     std::bind(&idEqrq, std::get<0>(*(std::get<0>(*j)))->getId(),
				       std::placeholders::_1));
    if (elem != readyQ->end()) {
      LOG(debug) << "Erasing job";
      readyQ->erase(elem);
    }
  }
  return j;
}
