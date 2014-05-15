#ifndef __ALLOCATIONS_H_
#define __ALOCATIONS_H_
#include <Job.hpp>


class Allocations {


  public:
    Allocations() {
      score = 0;
    }
    std::deque<std::unique_ptr<Job>> jobs;
    float score;
    float makespan() { float sum = 0; 
                       auto it = jobs.begin();
                       for(;it != jobs.end();it++) 
                       {
                          sum += (*it)->cost();
                       } 
                       return sum;}

};

#endif
