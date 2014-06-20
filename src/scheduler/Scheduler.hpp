#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/chrono.hpp>
#include <signal.h>
#include <unordered_map>
#include <MultiThreadedTCPServer.hpp>
#include <Resource.hpp>
#include <Job.hpp>
#include <typedefs.hpp>
#include <Allocations.hpp>
#include <Logging.hpp>

#include <concurrent_queue.hpp>

class Scheduler;
#define NUM_THREADS 4
#include <algs.hpp>
#include "ElasticityManager.hpp"

/** The scheduler is a server for the client API and a client of the dispatcher
 * **/


namespace bc = boost::chrono;

class Scheduler : public MultiThreadedTCPServer {
public:
  ~Scheduler() {
    bool already_cleaned = true;
    if (schedulerThread != nullptr) {
      LOG(debug) << "\t- Joining schedulerThread\n";
      schedulerThread->interrupt();
      schedulerThread->join();
      delete schedulerThread;
      already_cleaned = false;
    }
    if (dispatcherThread != nullptr) {
      LOG(debug) << "\t- Joining dispatcherThread\n";
      dispatcherThread->interrupt();
      dispatcherThread->join();
      delete dispatcherThread;
      already_cleaned = false;
    }
    if (finishedQThread != nullptr) {
      LOG(debug) << "\t- Joining finishedQThread\n";
      finishedQThread->interrupt();
      finishedQThread->join();
      delete finishedQThread;
      already_cleaned = false;
    }
    if (!already_cleaned) {
      LOG(debug) << "\t- Joining jobThreads\n";
      //      jobThreads.interrupt_all();
      ///      jobThreads.join_all();
    }
    std::cout << "(DEBUG): ~Scheduler() Deconstructed\n";
  }
  Scheduler(const ElasticityManager &_elasticityManager,
            const std::string &port, const std::string &name,
            int _dispatcherPortNumber, const std::string &_dispatcherHostname)
      : MultiThreadedTCPServer::super(name, port, NUM_THREADS),
        elasticityManager(_elasticityManager),
	dispatcherHostname(_dispatcherHostname),
	dispatcherPortNumber(_dispatcherPortNumber)
  {
    resPool = ResourcePoolPtr(new ResourcePool());
    readyQ = ConcurrentJobQueuePtr(new ConcurrentJobQueue());
    runQ = ConcurrentJobResPairQPtr(new ConcurrentJobResPairQ());
    finishedQ = ConcurrentJobQueuePtr(new ConcurrentJobQueue());

    resourceId = 1;
    addResource(dispatcherPortNumber, dispatcherHostname);

    nextJid = 1;
    strat = "Completion Time";
    window = 5;
    totalCompletions = 0;
    clientPriorities[1] = 1;
    clientPriorities[2] = 2;
    clientPriorities[3] = 3;
    clientPriorities[4] = 4;
    meanWaitTime = bc::seconds(0);
    meanServiceTime = bc::seconds(0);
    totalWaitTime = bc::seconds(0);
    totalServiceTime = bc::seconds(0);
    totalSchedTime = bc::seconds(0);

    totalLatency = bc::seconds(0);
    meanLatency = bc::seconds(0);
    meanThroughput = 0;
    meanUtilization = 0;
    numSchedules = 0;
    totalJobsScheduled = 0;
    startTime = bc::system_clock::now();
  }


  void addToRunQ(JobResPairPtr elem) {
    std::get<0>(*std::get<0>(*elem))->setDispatchTime(bc::system_clock::now());
    runQ->push_front(elem);
    LOG(debug) << "Added element to runQ, calling notify_all()";
  }

  /* Server functions */
  int getNextId() {
    boost::lock_guard<boost::mutex> lk(jidMtx);
    return nextJid++;
  }

  virtual msg_t *handle_request(msg_t *request);
  msg_t *concurrentHandler(msg_t &request, msg_t &response,
                           unsigned long responseSize);

  /* Thread functions */
  void schedLoop();
  void dispatcherLoop();

  virtual void start();

  void stop() {
    /* If not using priorities: */
    std::cout << std::dec << "(INFO): Uptime: "
              << bc::duration_cast<bc::seconds>(bc::system_clock::now() -
                                                startTime) << std::endl;
    std::cout << std::dec << "(INFO): Mean waiting Time: " << meanWaitTime
              << "\n";
    std::cout << std::dec << "(INFO): Mean Service Time: " << meanServiceTime
              << "\n";
    std::cout << std::dec << "(INFO): Mean Latency : " << meanLatency << "\n";
    std::cout << std::dec << "(INFO): Mean Throughput " << meanThroughput
              << "  (Jobs/sec)\n";
    std::cout << std::dec << "(INFO): Mean Utilization " << meanUtilization
              << "\n";
    std::cout << std::dec << "(INFO): Mean Scheduling Overhead "
              << totalSchedTime / numSchedules << "\n";
    std::cout << std::dec << "(INFO): Total Jobs : "
              << totalCompletions + readyQ->size() + runQ->size()
              << "\tTotal Completions: " << totalCompletions;
    std::cout << std::dec << "\tAvg. # Jobs/Schedule: "
              << (double)totalJobsScheduled / (double)numSchedules << "\n";
  };

  JobTuplePtr copyJobFromQ(ConcurrentJobQueuePtr rq, size_t i) {
    if (i > rq->size()) {
      return nullptr;
    }
    return rq->at(i);
  }

  inline std::string getStrategy() const { return strat; }
  inline size_t readyQSize() { return readyQ->size(); }
  inline size_t resPoolSize() { return resPool->size(); }
  inline size_t getWindow() const { return window; }

  /* Scheduling Strategies */
  Allocations *schedule(size_t, bool);
  void returnResources(Allocations &a);

  std::deque<Resource> allocate(Job &j, size_t m, size_t n);
  JobPtr realloc(JobPtr j);

  inline AlgVecType *getAlgVecPtr() { return &algVec; }
  inline void addSchedAlg(AlgType f) { algVec.push_back(f); }
  void runJob(JobResPairPtr j);

  inline static bool idEq(const int &jid, const JobResPairPtr &item) {
    return jid == std::get<0>(*std::get<0>(*item))->getId();
  }

  inline static bool idEqrq(const int &jid, const JobTuplePtr &item) {
    return jid == std::get<0>(*item)->getId();
  }

  JobResPairPtr removeJobFromReadyQ(const JobResPairPtr &j);
  void removeJobFromRunQ(int jid);

  ConcurrentJobQueuePtr getReadyQPtr() { return readyQ; }
  ConcurrentJobQueuePtr getFinishedQPtr() { return finishedQ; }
  ConcurrentJobResPairQPtr getRunQPtr() { return runQ; }

  void claimResources(JobResPairPtr elem);
  void returnResources(ResourceListPtr res);

  void updateStatistics(JobPtr j) {
    boost::lock_guard<boost::mutex> lk(waitTimeMtx);
    totalCompletions++;
    updateMeanWaitTime(j);
    updateMeanServiceTime(j);
    updateLatency(j);
    updateThroughput(j);
    updateUtilization(j);
    updateLateJobs(j);
  }

  void updateUtilization(JobPtr j) {
    totBusyTime += j->getActualExecutionTime();
    auto tp = bc::system_clock::now();
    auto totTimeMs = bc::duration_cast<bc::milliseconds>(tp - startTime);
    auto busyTimeMs = bc::duration_cast<bc::milliseconds>(totBusyTime);
    meanUtilization = (double)busyTimeMs.count() / (double)totTimeMs.count();
  }

  void updateMeanWaitTime(JobPtr j) {
    // LOCK
    totalWaitTime += (j->getDispatchTime() - j->getIssueTime());
    meanWaitTime = totalWaitTime / totalCompletions;
  }

  void updateMeanServiceTime(JobPtr j) {
    totalServiceTime += (j->getFinishTime() - j->getDispatchTime());
    meanServiceTime = totalServiceTime / totalCompletions;
  }

  void updateThroughput(JobPtr j) {
    auto tp = bc::system_clock::now();
    auto seconds = bc::duration_cast<bc::seconds>(tp - startTime);
    meanThroughput = (double)totalCompletions / (double)seconds.count();
  }

  void updateLateJobs(JobPtr j) {
    bc::duration<double> actualExecutionTime = bc::duration_cast<bc::seconds>(
        j->getFinishTime() - j->getDispatchTime());
    auto lateness = actualExecutionTime - estimateExecutionTime(j);
    LOG(debug) << "Estimate off by : " << lateness << " seconds\n";
  }

  void updateLatency(JobPtr j) {
    totalLatency += j->getMeasuredExecutionTime() - j->getActualExecutionTime();
    meanLatency = totalLatency / totalCompletions;
  }

  inline void addResource(Resource &r) {
    boost::lock_guard<boost::mutex> lk(resPoolMtx);
    int portNo = r.getPort();
    std::string hostName = r.getName();
    int Rid = r.getId();
    std::string type = r.getType();
    resPool->push_back(
        boost::shared_ptr<Resource>(new Resource(Rid, portNo, hostName, type)));
  }

  void provisionResource() {
    // TODO[paul-g] check resources are available
    LOG(debug) << "Adding resource to pool";
    addResource(dispatcherPortNumber, dispatcherHostname);
  }

  void deprovisionResource() {
    LOG(debug) << "Removing resource from pool";
  }

private:
  /* Setters */
  inline void setResPool(ResourcePoolPtr r) { resPool = r; }

  inline void setWindow(size_t w) { window = w; }

  inline void addResource(int PortNo, const std::string &Hostname) {
    boost::lock_guard<boost::mutex> lk(resPoolMtx);
    const std::string &type = std::string("DFE");
    resPool->push_back(
        boost::shared_ptr<Resource>(new Resource(resourceId, PortNo, Hostname, type)));
    resourceId++;
  }

  /* Getters */
  inline AlgType getAlg(size_t i) { return algVec[i]; }
  inline size_t noAlgs() { return algVec.size(); }

  /* Helper Functions */
  int addToReadyQ(msg_t &request);
  int getJobStatus(int jobID);
  void returnToReadyQ(JobPtr j, int pos);
  JobPtr estimateFinishTime(JobPtr j);
  bc::duration<double> estimateExecutionTime(JobPtr);
  int numLateJobs();
  void updateState();
  void dumpInfo();
  void reclaimResources();

  /* Helper functions for resource managment */
  void finishedLoop();
  void serviceAllocations(Allocations &a);

  /* Signaling Structs */
  boost::mutex qMutex;
  boost::condition_variable QCondVar;
  struct QInfo QStatus;

  /* used for locking Q's */

  boost::mutex jidMtx;
  boost::mutex waitTimeMtx;
  boost::mutex resPoolMtx;

  /* Worker threads */
  boost::thread *schedulerThread;
  boost::thread *dispatcherThread;
  boost::thread *finishedQThread;
  boost::thread_group jobThreads;

  /*Private Data Members */
  ResourcePoolPtr resPool;
  ConcurrentJobResPairQPtr runQ;
  ConcurrentJobQueuePtr finishedQ, readyQ;

  AlgVecType algVec;
  /* tuning variables */
  size_t window;
  bc::duration<double> meanWaitTime;
  bc::duration<double> totalWaitTime;

  bc::duration<double> meanServiceTime;
  bc::duration<double> totalServiceTime;
  bc::duration<double> meanLatency;
  bc::duration<double> totalLatency;
  bc::duration<long, boost::ratio<1l, 1000000l> > totalSchedTime;
  bc::duration<double, boost::ratio<1l, 1000000l> > totBusyTime;
  long numSchedules;
  long totalJobsScheduled;
  bc::time_point<bc::system_clock, bc::duration<double> > startTime;

  double meanThroughput;
  double meanUtilization;

  std::string strat;
  int nextJid;
  uint64_t totalCompletions;
  std::unordered_map<int, int> clientPriorities;

  int resourceId;
  int dispatcherPortNumber;
  const std::string &dispatcherHostname;

  const ElasticityManager &elasticityManager;
};

#endif /* _SCHEDULER_H_ */
