#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <MultiThreadedTCPServer.hpp>
#include <Resource.hpp>
#include <Job.hpp>
#include<Allocations.hpp>
class Scheduler;

#include <algs.hpp>

/** The scheduler is a server for the client API and a client of the dispatcher **/
//TODO Include Allocations and Job classes...

//TODO[mtottenh]: clean this up a bit
template <typename T>
class ContainerPtr {
  public:
    typedef std::shared_ptr< std::deque < T > >  deque;
};
/*
template <typename T>
class JobContainerPtr {
  public:
    typedef std::shared_ptr<std::deque<std::tuple<std::unique_ptr<T>, > >  deque;

};*/

// Simple data structure to indicate which Q an event occured on with atomic 
// get/set methods.
struct QInfo {
  bool readyQEvent;
  bool runQEvent;
  bool finishedQEvent;

  boost::mutex readyQStatusLock;
  boost::mutex runQStatusLock;
  boost::mutex finishedQStatusLock;

  bool getReadyQStatus() {
    boost::lock_guard<boost::mutex> l1(readyQStatusLock);
    return readyQEvent;
  }

  bool getRunQStatus() {
    boost::lock_guard<boost::mutex> l2(runQStatusLock);
    return runQEvent;
  }

  bool getFinishedQStatus() {
    boost::lock_guard<boost::mutex> l3(finishedQStatusLock);
    return finishedQEvent;
  }

  void setReadyQStatus(bool p) {
    boost::lock_guard<boost::mutex> l1(readyQStatusLock);
    readyQEvent = p;
  }
  void setRunQStatus(bool p) {
    boost::lock_guard<boost::mutex> l2(runQStatusLock);
    runQEvent = p;
  }

  void setFinishedQStatus(bool p) {
    boost::lock_guard<boost::mutex> l3(finishedQStatusLock);
    finishedQEvent = p;
  }

};


struct JobInfo {
  bool finished;
  boost::mutex jobInfoLock;

  bool isFinished() { 
    boost::lock_guard<boost::mutex> l1(jobInfoLock);
    return finished;
  }
  void setFinished(bool p) {
    boost::lock_guard<boost::mutex> l1(jobInfoLock);
    finished = p;
  }

};

class Scheduler : public MultiThreadedTCPServer {
  /* Algorithm Base and Collection Types */
  typedef std::function<Allocations(Scheduler &)> AlgType;
  typedef std::vector<AlgType> AlgVecType;
  /* Base Types */
  typedef std::unique_ptr<Job> JobPtr;
  typedef std::tuple<JobPtr, struct JobInfo&, boost::condition_variable&> JobTuple;
  typedef std::unique_ptr<Resource> ResourcePtr;
  typedef JobTuple* JobTuplePtr;
  /* Collections of Bases */
  typedef std::deque<ResourcePtr> ResourcePool;
  typedef std::deque<JobTuplePtr> JobQueue;
  /* Pointers to Collections */
  typedef std::shared_ptr<JobQueue> JobQueuePtr;
  typedef std::shared_ptr<ResourcePool> ResourcePoolPtr;


public:
  ~Scheduler() {
    schedulerThread->join();
    dispatcherThread->join();
    delete schedulerThread;
    delete dispatcherThread;
  }
  Scheduler(const std::string& port,
	    const std::string& name,
	    int dispatcherPortNumber,
	    const std::string& dispatcherHostname) :
    MultiThreadedTCPServer::super(name, port, 5)


  {
    resPool = ResourcePoolPtr(new ResourcePool());
    readyQ = JobQueuePtr(new JobQueue());
    runQ = JobQueuePtr(new JobQueue());
    finishedJobs = JobQueuePtr(new JobQueue());
    //TODO[mtottenh]: Change Resource so that it doesn't inherit from client
    addResource(dispatcherPortNumber,dispatcherHostname,1);

    nextJid = 1;
  }

  template <typename T> void enqueue(typename ContainerPtr<T*>::deque container, 
                                      T* elem, boost::mutex &lock, const std::string& name)
  {
    //Check that this shouldn't be the lock passed in.
    boost::lock_guard<boost::mutex> guard(qMutex);

    container->push_back(elem);
    //Change this to derive the name from Q ptri
    if (name == "readyQ")
      QStatus.setReadyQStatus(true);
    if (name == "runQ")
      QStatus.setRunQStatus(true);
    if (name == "finishedQ")
      QStatus.setFinishedQStatus(true);
    std::cout << "Added element to " << name << ", calling notify_all().\n";
    QCondVar.notify_all();

  }

  /* Server functions */
  //TODO[mtottenh]:add locks to this
  int getNextId() { return nextJid++;}
  virtual msg_t* handle_request(msg_t* request);
  virtual void defaultHandler(msg_t& request, msg_t& response, int responseSize);
  msg_t* concurrentHandler(msg_t& request, msg_t& response, int responseSize);
  void schedLoop();
  void dispatcherLoop();
  virtual void start();

  virtual void stop();

  Job getJobFromQ(JobQueuePtr rq, int i)
  {
    auto j = *std::get<0>(*(rq->at(i)));
    return j;
  }
  inline std::string getStrategy() const { return strat; }
  inline int  readyQSize(){return readyQ->size();}
  inline int  resPoolSize(){return resPool->size();}
  inline int    getWindow() const { return window;}
  /* Scheduling Strategies */
  void schedule();
  void schedule(int);
  Allocations schedule(int,bool);
  void returnResources(Allocations &a);

  inline  JobQueuePtr getQueuePointer(std::string s)
  {
    if (s.compare("readyQ") == 0)
        return readyQ;
    if (s.compare("runQ") == 0)
        return runQ;
    if (s.compare("finishedJobs") == 0)
        return finishedJobs;
    return nullptr;
  }
  Job  allocate(Job &j, int m, int n);
  inline  AlgVecType* getAlgVecPtr() { return &algVec;}
  JobPtr realloc(JobPtr j);
  inline void addSchedAlg(AlgType f) { algVec.push_back(f);}
  void addToRunQ(JobPtr j);

  Job removeJobFromQ(JobQueuePtr jq, Job& j) {
    JobQueuePtr preserve_list = JobQueuePtr(new JobQueue());
    auto a = jq->begin();
    for(;a != jq->end(); a++) {
      if (std::get<0>(**a)->getId() != j.getId()) {
        preserve_list->push_back(*a);
      }
    }
    jq = std::move(preserve_list);
    return j;
  }

  template <typename T> T removeFromQ(typename ContainerPtr<T>::deque jq, T j) {
    typename ContainerPtr<T>::deque preserve_list;
    auto a = jq->begin();
    for(;a != jq->end(); a++) {
      if ((*a)->getId() != j->getId()) {
        preserve_list->push_back( move(*a) );
      }
    }
    jq = preserve_list;
    return j;
  }





private:
  /* Setters */
  inline void setReadyQ(JobQueuePtr rq) {readyQ = rq;}
  inline void setRunQ(JobQueuePtr rq) {runQ = rq;}
  inline void setResPool(ResourcePoolPtr r) {resPool=r;}
  inline void setWindow(int w) {window = w;}
  inline void setMaxTime(float m) {maxTime = m;}
  inline void setNextJobTime(float njt) { nextJobTime = njt;}

  inline void addResource(int PortNo, const std::string& Hostname, int Rid)
  {
        resPool->push_back(std::unique_ptr<Resource>(new Resource(PortNo,Hostname,Rid)));
  }


  /* Getters */

  inline float  getCurTime() const {return curTime;}
  inline float  getNextJobTime() const {return nextJobTime;}

  inline  AlgType getAlg(int i) { return algVec[i];}
  inline  int noAlgs() {return algVec.size();}

  inline  JobTuplePtr getFirstReadyProc()
  {
    JobTuplePtr j = std::move( readyQ->front() ) ;
    readyQ->pop_front();
    return j;
  }


  /* Helper Functions */
  int addToReadyQ(msg_t& request);

  void kickStartRunQ();
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

  JobPtr deallocate(JobPtr j);

  void serviceAllocations(Allocations &a);

  /* used for signaling scheduler&dispatcher threads */
  boost::mutex qMutex;
  boost::condition_variable QCondVar;
  struct QInfo QStatus;
  /* used for locking Q's */
  
  boost::mutex readyQMtx;
  boost::mutex runQMtx;
  boost::mutex finishedQMtx;

  boost::thread *schedulerThread;
  boost::thread *dispatcherThread;

  /*Private Data Members */
  ResourcePoolPtr resPool;
  JobQueuePtr readyQ;
  JobQueuePtr runQ;
  JobQueuePtr finishedJobs;
  AlgVecType algVec;
  int window;
  float maxTime;
  float curTime;
  float nextJobTime;
  std::string strat;
  int nextJid;
};

#endif /* _SCHEDULER_H_ */
