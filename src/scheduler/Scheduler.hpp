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

#include <unordered_map>
#include <MultiThreadedTCPServer.hpp>
#include <Resource.hpp>
#include <Job.hpp>
#include <typedefs.hpp>
#include <Allocations.hpp>
class Scheduler;
#define NUM_THREADS 10
#include <algs.hpp>

/** The scheduler is a server for the client API and a client of the dispatcher **/

//TODO[mtottenh]: clean this up a bit
template <typename T>
class ContainerPtr {
  public:
    typedef std::shared_ptr< std::deque < T > >  deque;
};

class Scheduler : public MultiThreadedTCPServer {

public:
  ~Scheduler() {
    schedulerThread->interrupt();
    schedulerThread->join();
    dispatcherThread->interrupt();
    dispatcherThread->join();
    jobThreads.join_all();
    delete schedulerThread;
    delete dispatcherThread;
  }
  Scheduler(const std::string& port,
	    const std::string& name,
	    int dispatcherPortNumber,
	    const std::string& dispatcherHostname) :
    MultiThreadedTCPServer::super(name, port, NUM_THREADS)
  {
    resPool = ResourcePoolPtr(new ResourcePool());
    readyQ = JobQueuePtr(new JobQueue());
    runQ = JobResPairQPtr(new JobResPairQ());
    finishedQ = JobQueuePtr(new JobQueue());
    //TODO[mtottenh]: Change Resource so that it doesn't inherit from client
    addResource(dispatcherPortNumber,dispatcherHostname,1);
    addResource(dispatcherPortNumber,dispatcherHostname,2);
    addResource(dispatcherPortNumber,dispatcherHostname,3);
    addResource(dispatcherPortNumber,dispatcherHostname,4);

    nextJid = 1;
    strat = "Completion Time";
    window = 5;
    totalJobs = 0;
    clientPriorities[1] = 1;
    clientPriorities[2] = 2;
    clientPriorities[3] = 3;
    clientPriorities[4] = 4;
    meanWaitTime = boost::chrono::seconds(0); 
  }

  template <typename T> void enqueue(typename ContainerPtr<std::shared_ptr<T>>::deque container, 
                                      std::shared_ptr<T> elem, boost::mutex &lock, const std::string& name)
  {
    //Check that this shouldn't be the lock passed in.
  //  std::cout << "Scheduler::enqueue()\n";
    boost::lock_guard<boost::mutex> guard(lock);

    container->push_back(elem);
    if (name == "readyQ") {
      std::get<0>(*elem)->setIssueTime( boost::chrono::system_clock::now());
      QStatus.setReadyQStatus(true);
    }
    if (name == "runQ")
      QStatus.setRunQStatus(true);
    if (name == "finishedQ") {
      std::get<0>(*elem)->setFinishTime( boost::chrono::system_clock::now());
      QStatus.setFinishedQStatus(true);
    }
#ifdef DEBUG
    std::cout << "Added element to " << name << ", calling notify_all().\n";
#endif
    QCondVar.notify_all();
  }
  void addToRunQ(JobResPair &elem) {
    boost::lock_guard<boost::mutex> guard(runQMtx);
#ifdef DEBUG
    std::cout << "Adding " << *std::get<0>(*std::get<0>(elem)) << "To RunQ\n";
#endif
    runQ->push_back(elem);
    std::get<0>(*std::get<0>(elem))->setDispatchTime(boost::chrono::system_clock::now());
    QStatus.setRunQStatus(true);
#ifdef DEBUG
    std::cout << "Added element to runQ, calling notify_all().\n";
#endif
    QCondVar.notify_all();
  }
  /* Server functions */
  int getNextId() { 
    boost::lock_guard<boost::mutex> lk(jidMtx);
    return nextJid++;
  }

  virtual msg_t* handle_request(msg_t* request);
  virtual void defaultHandler(msg_t& request, msg_t& response, int responseSize);
  msg_t* concurrentHandler(msg_t& request, msg_t& response, unsigned long responseSize);

   
  void notifyClientsOfResults();
  /* Thread functions */
  void schedLoop();
  void dispatcherLoop();

  virtual void start();
  void stop() {   
/*    schedulerThread->interrupt();
    schedulerThread->join();
    dispatcherThread->interrupt();
    dispatcherThread->join();
    jobThreads.join_all();
    delete schedulerThread;
    delete dispatcherThread;*/
  } ;

  JobTuplePtr copyJobFromQ(JobQueuePtr rq, size_t i)
  { 
    if (i > rq->size()) {
      return nullptr;
    }
//    JobTuple j = *(rq->at(i));
//    JobTuplePtr nj = std::shared_ptr<JobTuple>(new JobTuple(std::get<0>(j), std::get<1>(j), std::get<2>(j)));
    return rq->at(i);
  }
  inline std::string getStrategy() const { return strat; }
  inline size_t readyQSize(){return readyQ->size();}
  inline size_t resPoolSize(){return resPool->size();}
  inline size_t    getWindow() const { return window;}
  /* Scheduling Strategies */
  Allocations* schedule(size_t,bool);
  void returnResources(Allocations &a);
  
  std::deque<Resource>  allocate(Job &j, size_t m, size_t n);
  JobPtr realloc(JobPtr j);

  inline  AlgVecType* getAlgVecPtr() { return &algVec;}
  inline void addSchedAlg(AlgType f) { algVec.push_back(f);}
  void runJobs();
  void runJob(JobResPair& j);
  
  inline static bool idEq (const int& jid, const JobResPair & item) {
    return jid == std::get<0>(*std::get<0>(item))->getId();
  }

  inline static bool idEqrq (const int& jid, const JobTuplePtr & item) {
    return jid == std::get<0>(*item)->getId();
  }
  JobResPair removeJobFromReadyQ(const JobResPair& j) {
    boost::lock_guard<boost::mutex> lk(readyQMtx);
/*    if (readyQ->size() > 0) {
      auto elem = std::find_if(readyQ->begin(), readyQ->end(),
                             std::bind(&idEqrq, std::get<0>(*(std::get<0>(j)))->getId(),
                                       std::placeholders::_1));
      if (elem != readyQ->end()) {
        readyQ->erase(elem);
      }
*/
    JobQueuePtr preserve_list = JobQueuePtr(new JobQueue());
    JobQueue::iterator a = readyQ->begin();
    for(;a != readyQ->end(); a++) {
      if (std::get<0>(**a)->getId() != std::get<0>(*(std::get<0>(j)))->getId()) {
        preserve_list->push_back(*a);
      }
    }
    readyQ = preserve_list;
    
    return j;
  }
  void removeJobFromRunQ(int jid);

  /* TODO[mtottenh]: Deprecate these. access to Ptrs of these Q's is bad 
   * now that everything is multithreaded
   */
  JobQueuePtr getReadyQPtr() { return readyQ;}
  JobQueuePtr getFinishedQPtr() { return finishedQ; }
  JobResPairQPtr getRunQPtr() { return runQ;}

  void claimResources(JobResPair& elem);
  void returnResources(ResourceList& res);
  void updateMeanWaitTime(JobPtr j) {
    //LOCK
    boost::lock_guard<boost::mutex> lk(waitTimeMtx);
    totalJobs++;
    meanWaitTime +=  (j->getDispatchTime() - j->getIssueTime());
    meanWaitTime /=  totalJobs;  
  }
  inline void addResource(Resource& r){
    boost::lock_guard<boost::mutex> lk(resPoolMtx);
    int portNo = r.getPort(); 
    std::string hostName = r.getName();
    int Rid = r.getId();
    std::string type = r.getType();
    resPool->push_back(std::unique_ptr<Resource>(new Resource(Rid,portNo,hostName,type)));
  }
private:
  JobPtr deallocate(JobPtr j);
  /* Setters */
  inline void setReadyQ(JobQueuePtr rq) {readyQ = rq;}
  inline void setRunQ(JobResPairQPtr rq) {runQ = rq;}
  inline void setResPool(ResourcePoolPtr r) {resPool=r;}

  inline void setWindow(size_t w) {window = w;}


  inline void addResource(int PortNo, const std::string& Hostname, int Rid)
  {
    boost::lock_guard<boost::mutex> lk(resPoolMtx);
    const std::string& type = std::string("DFE");
    resPool->push_back(std::unique_ptr<Resource>(new Resource(Rid,PortNo,Hostname,type)));
  }


  /* Getters */
  //inline float  getCurTime() const {return curTime;}
  inline  AlgType getAlg(size_t i) { return algVec[i];}
  inline  size_t noAlgs() {return algVec.size();}
  inline  JobTuplePtr getFirstReadyProc()
  {
    boost::lock_guard<boost::mutex> lk(readyQMtx);
    JobTuplePtr j = std::move( readyQ->front() ) ;
    readyQ->pop_front();
    return j;
  }

  /* Helper Functions */
  int addToReadyQ(msg_t& request);
  int getJobStatus(int jobID);
  msg_t getJobResponse(int);
  void returnToReadyQ(JobPtr j,int pos);
  JobPtr estimateFinishTime(JobPtr j);
  int numLateJobs();
  void updateState();
  void dumpInfo();
  void printQInfo(const char*, JobQueuePtr, bool);
  void reclaimResources();
  /* Attempts to allocate a least n resources to job j
   * up to a maximum of m resources
   * returns # resources allocated or -1 on failure
   */
  /* Helper functions for resource managment */



  void serviceAllocations(Allocations &a);

  /* Signaling Structs */
  boost::mutex qMutex;
  boost::condition_variable QCondVar;
  struct QInfo QStatus;
  /* used for locking Q's */
  boost::mutex readyQMtx;
  boost::mutex runQMtx;
  boost::mutex finishedQMtx;
  boost::mutex jidMtx;
  boost::mutex waitTimeMtx;
  boost::mutex resPoolMtx;

  /* Worker threads */
  boost::thread *schedulerThread;
  boost::thread *dispatcherThread;
  boost::thread_group jobThreads;
  /*Private Data Members */
  ResourcePoolPtr resPool;
  JobQueuePtr readyQ;
  JobResPairQPtr runQ;
  JobQueuePtr finishedQ;

  AlgVecType algVec;
  /* tuning variables */
  size_t window;
  boost::chrono::duration<double> meanWaitTime;
  std::string strat;
  int nextJid;
  uint64_t totalJobs;
  std::unordered_map<int,int> clientPriorities;
};

#endif /* _SCHEDULER_H_ */
