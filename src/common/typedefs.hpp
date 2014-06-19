#ifndef __TYPEDEFS__HPP__
#define __TYPEDEFS__HPP__
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include <concurrent_queue.hpp>

class Allocations;
class Scheduler;
class Job;
class Resource;

/* Deleter for boost::unqiue_ptr
 * From - http://stackoverflow.com/questions/3873750/boost-unique-ptr-deletor
 */
template <typename T> struct Deleter {
  void operator()(T *p) { delete p; }
};
struct JobInfo {
  bool finished;
  bool started;
  boost::mutex finishedLock;
  boost::mutex startedLock;
  bool isFinished() {
    boost::lock_guard<boost::mutex> l1(finishedLock);
    return finished;
  }
  void setFinished(bool p) {
    boost::lock_guard<boost::mutex> l1(finishedLock);
    finished = p;
  }
  bool isStarted() {
    boost::lock_guard<boost::mutex> l1(startedLock);
    return started;
  }
  void setStarted(bool p) {
    boost::lock_guard<boost::mutex> l1(startedLock);
    started = p;
  }
};
/* Base Types */
// typedef boost::unique_ptr<Resource,Deleter<Resource>> ResourcePtr;
typedef boost::shared_ptr<Resource> ResourcePtr;
// typedef std::unique_ptr<Job> JobPtr;
typedef boost::shared_ptr<Job> JobPtr;
typedef boost::shared_ptr<struct JobInfo> JobInfoPtr;
typedef boost::shared_ptr<boost::condition_variable> CondVarPtr;
typedef std::tuple<JobPtr, JobInfoPtr, CondVarPtr> JobTuple;
typedef std::shared_ptr<JobTuple> JobTuplePtr;
typedef std::deque<Resource> ResourceList; // TODO: change this maybe?
typedef std::shared_ptr<ResourceList> ResourceListPtr;
typedef std::pair<JobTuplePtr, ResourceListPtr> JobResPair;
typedef std::shared_ptr<JobResPair> JobResPairPtr;

/* Collections of Bases */
typedef std::deque<ResourcePtr> ResourcePool;
typedef std::deque<JobTuplePtr> JobQueue;
typedef concurrent_queue<JobTuplePtr> ConcurrentJobQueue;
typedef std::deque<JobResPairPtr> JobResPairQ;

/* Pointers to Collections */
typedef std::shared_ptr<JobQueue> JobQueuePtr;
typedef std::shared_ptr<ConcurrentJobQueue> ConcurrentJobQueuePtr;
typedef std::shared_ptr<ResourcePool> ResourcePoolPtr;
typedef std::shared_ptr<JobResPairQ> JobResPairQPtr;

/* Algorithm Base and Collection Types */
typedef std::function<Allocations *(Scheduler &)> AlgType;
typedef std::vector<AlgType> AlgVecType;
typedef std::function<float(JobResPair &)> CostFunctionType;

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

#endif
