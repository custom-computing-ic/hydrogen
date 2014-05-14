#ifndef _JOB_H_
#define _JOB_H_
#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <Resource.hpp>

class Job {
	public:

    Job(){
       issueTime = 0;
       dispatchTime = 0;
       finishTime = 0;
    };
    Job(int a,int b,std::function<float(Job&)> c);
    Job(int a, int b, std::function<float(Job&)> c, float d);
    /* getters */
    std::string str() const;
    inline int getMin() const { return min;} 
    inline int getMax() const { return max;} 
    inline int getJid() const { return jid;} 
    inline int getPriority() const { return priority; }
    inline std::function<float(Job&)> getCostFunc() { return cost_func;}
    inline float getDispatchTime()const {return dispatchTime;}
    inline float getIssueTime() const {return issueTime;}
    inline float getFinishTime() const {return finishTime;}
    inline float getDefaultJobTime() const {return defaultJobTime;}
    inline alocResList* getAllocdRes() {return &allocdResources;}
    /* setters */
    inline void setIssueTime(float a) {std::cout << *this << " issued at: " << a << "\n"; issueTime = a;}
    inline void setDispatchTime(float a) { std::cout << *this << " dispatched at: " << a << "\n"; dispatchTime = a;}
    inline void setFinishTime() { finishTime = dispatchTime + this->cost(); std::cout << *this << "Will finish at: " << finishTime << "\n";}
    inline void setMin(int a) {min = a;}
    inline void setUid(int a) {uid = a;}

    inline void setMax(int a) {max = a;}
    inline void setJid(int a) {jid = a;}
    inline void setCostFunc(std::function<float(Job&)> f) {cost_func = f;}


    inline float cost() { return cost_func(*this);}

    inline float minCost() const { return defaultJobTime / min;}
    inline float maxCost() const { return defaultJobTime / max;}
    inline void allocate(Resource a ) { allocdResources.push_back(a); }
    
    friend bool operator ==(const Job& lhs, const Job& rhs) { return lhs.getJid() == rhs.getJid();}
    friend std::ostream & operator<<(std::ostream & s, Job const & j) {
      s << "Job["<<j.getJid()<<"]";
      return s;
    }
  private:
  	int min;
		int max;
    int jid;
    int uid;
    int priority;
    float defaultJobTime;
    float issueTime;
    float dispatchTime;
    float finishTime;
    std::function<float(Job&)> cost_func;
    std::vector<Resource> allocdResources;

};
#endif
