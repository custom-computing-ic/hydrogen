#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <Server.hpp>
#include <Resource.hpp>
#include <Job.hpp>
#include<Allocations.hpp>
class Scheduler;

#include <algs.hpp>

/** The scheduler is a server for the client API and a client of the dispatcher **/
//TODO Include Allocations and Job classes...

//typedef int Job;
//TODO[mtottenh]: clean this up a bit
  template <typename T> 
    class ContainerPtr {
      public:
      typedef std::shared_ptr< std::deque < T > >  deque;
    };
class Scheduler : public Server {
  /* Algorithm Base and Collection Types */
  typedef std::function<Allocations(Scheduler &)> AlgType;
  typedef std::vector<AlgType> AlgVecType;

  /* Base Types */
  typedef std::shared_ptr<Resource> ResourcePtr;
  typedef std::shared_ptr<Job> JobPtr;
  
  /* Collections of Bases */
  typedef std::deque<ResourcePtr> ResourcePool;
  typedef std::deque<JobPtr> JobQueue;

  /* Pointers to Collections */
  typedef std::shared_ptr<JobQueue> JobQueuePtr;
  typedef std::shared_ptr<ResourcePool> ResourcePoolPtr;


public:
  Scheduler(int port,
	    const std::string& name,
	    int dispatcherPortNumber,
	    const std::string& dispatcherHostname) :
    Server::super(port, name)
  {
    resPool = ResourcePoolPtr(new ResourcePool());
    readyQ = JobQueuePtr(new JobQueue()); 
    runQ = JobQueuePtr(new JobQueue()); 
    finishedJobs = JobQueuePtr(new JobQueue()); 
    addResource(dispatcherPortNumber,dispatcherHostname,1);


  }
  /* Server functions */
  virtual void handleRequest(msg_t& request, msg_t& response);
  virtual void defaultHandler(msg_t& request, msg_t& response);
  virtual void start();
  
  virtual void stop();

  Job getJobFromQ(JobQueuePtr rq, int i) 
  {
    return *(*rq)[i];   
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
    return NULL;
  }
  int  allocate(Job &j, int m, int n);
  inline  AlgVecType* getAlgVecPtr() { return &algVec;}
  void realloc(Job& j);
  inline void addSchedAlg(AlgType f) { algVec.push_back(f);}
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
        resPool->push_back(std::make_shared<Resource>(PortNo,Hostname,Rid));
  }

 
  /* Getters */

  inline float  getCurTime() const {return curTime;}
  inline float  getNextJobTime() const {return nextJobTime;}

  inline  AlgType getAlg(int i) { return algVec[i];}
  inline  int noAlgs() {return algVec.size();}
 
  inline  Job getFirstReadyProc() 
  { 
    Job j = *(readyQ->front());
    readyQ->pop_front();
    return j;
  }


  /* Helper Functions */ 
  virtual void addToReadyQ(msg& request, msg_t& response);
  void addToRunQ(Job& j);

  void returnToReadyQ(Job& j,int pos);
  int estimateFinishTime(Job& j);
  int numLateJobs();
  void updateState();
  void dumpInfo();
  void printQInfo(const char*, JobQueuePtr, bool);
  void reclaimResources();
  template <typename T> void removeFromQ(typename ContainerPtr<T>::deque, T);


  /* Attempts to allocate a least n resources to job j
   * up to a maximum of m resources
   * returns # resources allocated or -1 on failure
   */
  /* Helper functions for resource managment */

  void deallocate(Job& j);

  void serviceAllocations(Allocations &a);



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
};

#endif /* _SCHEDULER_H_ */
