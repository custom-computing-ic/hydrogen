#ifndef __ALLOCATIONS_H_
#define __ALOCATIONS_H_
#include <Job.hpp>


class Allocations {


  public:
    Allocations() {
      score = 0;
    }
    std::deque<Job> jobs;
    float score;
    float makespan() { float sum = 0; 
                       std::deque<Job>::iterator it = jobs.begin();
                       for(;it<jobs.end();it++) 
                       {
                          sum += it->cost();
                       } 
                       return sum;}

};

#endif
