#ifndef _JOB_H_
#define _JOB_H_
#include <iostream>
#include <sstream>
#include <functional>
#include <memory>
#include <deque>
#include <vector>
#include <Resource.hpp>

class Job {

  /* Base Types */
  typedef std::unique_ptr<Resource> ResourcePtr;
  typedef std::unique_ptr<Job> JobPtr;

  /* Collections of Bases */
  typedef std::deque<ResourcePtr> ResourcePool;
  typedef std::deque<JobPtr> JobQueue;

  /* Pointers to Collections */
  typedef std::shared_ptr<JobQueue> JobQueuePtr;
  typedef std::shared_ptr<ResourcePool> ResourcePoolPtr;


	public:

    Job(){
       issueTime = 0;
       dispatchTime = 0;
       finishTime = 0;
       AllocatedResources = ResourcePoolPtr(new ResourcePool());
    };
    Job(msg_t& request,int);
   /* {
      issueTime = 0;
      dispatchTime = 0;
      finishTime = 0;
  
      AllocatedResources = std::make_shared<ResourcePool>();
    }*/
    Job(int a,int b,std::function<float(Job&)> c);
    Job(int a, int b, std::function<float(Job&)> c, float d);
    /* getters */
    std::string str() const;
    inline int getStatus() const { return status; }
    inline int noAllocdRes() const { return AllocatedResources->size();}
    inline int getMin() const { return min;} 
    inline int getMax() const { return max;} 
    inline int getId() const { return jid;} 
    inline int getPriority() const { return priority; }
    inline std::function<float(Job&)> getCostFunc() { return cost_func;}
    inline float getDispatchTime()const {return dispatchTime;}
    inline float getIssueTime() const {return issueTime;}
    inline float getFinishTime() const {return finishTime;}
    inline float getDefaultJobTime() const {return defaultJobTime;}
    inline ResourcePoolPtr getAllocdRes() {return AllocatedResources;}
    /* setters */
    inline void setIssueTime(float a) {std::cout << *this << " issued at: " << a << "\n"; issueTime = a;}
    inline void setDispatchTime(float a) { std::cout << *this << " dispatched at: " << a << "\n"; dispatchTime = a;}
    inline void setFinishTime() { finishTime = dispatchTime + this->cost(); std::cout << *this << "Will finish at: " << finishTime << "\n";}
    inline void setMin(int a) {min = a;}
    inline void setUid(int a) {uid = a;}

    inline void setMax(int a) {max = a;}
    inline void setJid(int a) {jid = a;}
    inline void setCostFunc(std::function<float(Job&)> f) {cost_func = f;}
    inline void setStatus(int a) { status = a;}
    inline float cost() { return cost_func(*this);}

    inline float minCost() const { return defaultJobTime / min;}
    inline float maxCost() const { return defaultJobTime / max;}

    inline void allocate(ResourcePtr a) { AllocatedResources->push_back(move(a)); }
    
    friend bool operator ==(const Job& lhs, const Job& rhs) { return lhs.getId() == rhs.getId();}
    friend std::ostream & operator<<(std::ostream & s, Job const & j) {
      s << "Job["<<j.getId()<<"]";
      return s;
    }
    msg_t run();
    void getResponse(char*,int);
  private:
  	int min;
		int max;
    int jid;
    int uid;
    int status;
    int priority;
    float defaultJobTime;
    float issueTime;
    float dispatchTime;
    float finishTime;

    msg_t req;
    std::function<float(Job&)> cost_func;
    ResourcePoolPtr AllocatedResources;

};
#endif
