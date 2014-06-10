#ifndef _JOB_H_
#define _JOB_H_
#include <iostream>
#include <sstream>
#include <functional>

#include <boost/chrono.hpp>
#include <boost/make_shared.hpp>
#include <memory>
#include <deque>
#include <vector>
#include <typedefs.hpp>
#include <Resource.hpp>
float defaultCostFunction(JobResPair& p);

class Job {
	public:
    /* Constructors */
    Job(msg_t* request,int);
    /* getters */
    ~Job(){
      if (req != nullptr) {
       req = nullptr;
      }
      if (rsp != nullptr) {
        rsp = nullptr;
      }
    }
    std::string str() const;
    msg_t* getReq() { return req;}
    void   setRsp(msg_t* r) { this->rsp = r; }
    void   copyRsp(char* buff,size_t sizeBytes) {
      msg_t* r = (msg_t*)buff;
//      this->rsp = std::make_shared<msg_t>();
            
      memcpy(this->rsp->data,r->data,r->dataBytes());
    }
    inline int getStatus() const { return status; }
    inline size_t getMin() const { return min;}
    inline size_t getMax() const { return max;}
    inline int getId() const { return jid;}
    inline int getPriority() const { return priority; }

    inline CostFunctionType getCostFunc() { return cost_func;}

    inline boost::chrono::system_clock::time_point getDispatchTime()const {return dispatchTime;}
    inline boost::chrono::system_clock::time_point getIssueTime() const {return issueTime;}
    inline boost::chrono::system_clock::time_point getFinishTime() const {return finishTime;}
    inline float getDefaultJobTime() const {return defaultJobTime;}
    
    /* setters */
    inline void setIssueTime(boost::chrono::system_clock::time_point a) { 
    //  std::cout << *this << " Issued\n";// at: " << a << "\n"; 
      issueTime = a;
    }
    inline void setDispatchTime(boost::chrono::system_clock::time_point a) { 
//      std::cout << *this << " Dispatched\n";// at: " << a << "\n"; 
      dispatchTime = a;
    }
    inline void setFinishTime(boost::chrono::system_clock::time_point a) { 
  //    std::cout << *this << " Finished\n"; // << a << "\n";
      finishTime = a; 
    }
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
    msg_t* getRsp() { return rsp;}
    void getResponse(char*,size_t);

  private:
  	size_t min;
	  size_t max;
    int jid;
    int uid;
    int status;
    int priority;
    float defaultJobTime;
    
    
    boost::chrono::system_clock::time_point issueTime;
    boost::chrono::system_clock::time_point dispatchTime;
    boost::chrono::system_clock::time_point finishTime;



    CostFunctionType cost_func;
    ResourcePoolPtr AllocatedResources;
    //TODO[paul-g]: What is the best way of copying over a msg_t ?
    //Assigning it to a ref (e.g. msg_t& req and assigning in the constructor)
    //seems to lead to data corruption (probably because the pointer data
    //isn't a fixed size.
    msg_t* req;
    msg_t* rsp;

    //
//    std::shared_ptr<msg_t> req;
 //   std::shared_ptr<msg_t> rsp;
};
#endif
