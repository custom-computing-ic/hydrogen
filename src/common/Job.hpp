#ifndef _JOB_H_
#define _JOB_H_
#include <iostream>
#include <sstream>
#include <functional>
#include <memory>
#include <deque>
#include <vector>
#include <typedefs.hpp>
#include <Resource.hpp>
float defaultCostFunction(JobResPair& p);

class Job {

	public:
    /* Constructors */
//    Job(){
//       issueTime = 0;
//       dispatchTime = 0;
//       finishTime = 0;
//       AllocatedResources = ResourcePoolPtr(new ResourcePool());
//   }
    Job(msg_t& request,int);
//    Job(int a,int b,CostFunctionType c);
//    Job(int a, int b, CostFunctionType c, float d);
    /* getters */
    std::string str() const;
    msg_t& getReq() { return req;}
    inline int getStatus() const { return status; }
    inline size_t getMin() const { return min;}
    inline size_t getMax() const { return max;}
    inline int getId() const { return jid;}
    inline int getPriority() const { return priority; }

    inline CostFunctionType getCostFunc() { return cost_func;}

    inline float getDispatchTime()const {return dispatchTime;}
    inline float getIssueTime() const {return issueTime;}
    inline float getFinishTime() const {return finishTime;}
    inline float getDefaultJobTime() const {return defaultJobTime;}
    
    /* setters */
    inline void setIssueTime(float a) { 
      std::cout << *this << " issued at: " << a << "\n"; 
      issueTime = a;
    }
    inline void setDispatchTime(float a) { 
      std::cout << *this << " dispatched at: " << a << "\n"; 
      dispatchTime = a;
    }
//    inline void setFinishTime() { 
//      finishTime = dispatchTime + this->cost(); 
//      std::cout << *this << "Will finish at: " << finishTime << "\n";
//    }
    inline void setMax(size_t a) {max = a;}
    inline void setMin(size_t a) {min = a;}
    inline void setUid(int a) {uid = a;}

    inline void setJid(int a) {jid = a;}
    inline void setCostFunc(CostFunctionType f) {cost_func = f;}
    inline void setStatus(int a) { status = a;}

    inline float cost(JobResPair& p) { return cost_func(p);}


    inline float minCost() const { return defaultJobTime / min;}
    inline float maxCost() const { return defaultJobTime / max;}


    friend bool operator ==(const Job& lhs, const Job& rhs) {
      return lhs.getId() == rhs.getId();
    }
    friend std::ostream & operator<<(std::ostream & s, Job const & j) {
      s << "Job["<<j.getId()<<"]";
      return s;
    }
    /* Helper Functions */
    msg_t run();
    void getResponse(char*,size_t);

  private:
  	size_t min;
	  size_t max;
    int jid;
    int uid;
    int status;
    int priority;
    float defaultJobTime;
    float issueTime;
    float dispatchTime;
    float finishTime;


    CostFunctionType cost_func;
    ResourcePoolPtr AllocatedResources;
    //TODO[paul-g]: What is the best way of copying over a msg_t ?
    //Assigning it to a ref (e.g. msg_t& req and assigning in the constructor)
    //seems to lead to data corruption (probably because the pointer data
    //isn't a fixed size.
    msg_t& req;

};
#endif
