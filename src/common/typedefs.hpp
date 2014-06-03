#ifndef __TYPEDEFS__HPP__
#define __TYPEDEFS__HPP__
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
class Allocations;
class Scheduler;
class Job;
class Resource;

/* Base Types */
typedef std::unique_ptr<Resource> ResourcePtr;
//typedef std::unique_ptr<Job> JobPtr;
typedef std::shared_ptr<Job> JobPtr;
typedef std::tuple<JobPtr, struct JobInfo&, boost::condition_variable&> JobTuple;
typedef std::shared_ptr<JobTuple> JobTuplePtr;
typedef std::deque<Resource> ResourceList;   //TODO: change this maybe?
typedef std::pair<JobTuplePtr,ResourceList> JobResPair;

/* Collections of Bases */
typedef std::deque<ResourcePtr> ResourcePool;
typedef std::deque<JobTuplePtr> JobQueue;
typedef std::deque<JobResPair>  JobResPairQ;

/* Pointers to Collections */
typedef std::shared_ptr<JobQueue>     JobQueuePtr;
typedef std::shared_ptr<ResourcePool> ResourcePoolPtr;
typedef std::shared_ptr<JobResPairQ>  JobResPairQPtr;


/* Algorithm Base and Collection Types */
typedef std::function<Allocations* (Scheduler &)> AlgType;
typedef std::vector<AlgType> AlgVecType;
typedef std::function<float(JobResPair&) > CostFunctionType;




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
  bool started;
  boost::mutex jobInfoLock;
  
  bool isFinished() { 
    boost::lock_guard<boost::mutex> l1(jobInfoLock);
    return finished;
  }
  void setFinished(bool p) {
    boost::lock_guard<boost::mutex> l1(jobInfoLock);
    finished = p;
  }
  bool isStarted() { 
    boost::lock_guard<boost::mutex> l1(jobInfoLock);
    return started;
  }
  void setStarted(bool p) {
    boost::lock_guard<boost::mutex> l1(jobInfoLock);
    started = p;
  }
};





#endif
