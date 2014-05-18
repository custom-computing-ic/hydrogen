#ifndef __ALLOCATIONS_H_
#define __ALLOCATIONS_H_
#include <Job.hpp>
#include <typedefs.hpp>
class Allocations {
 


  public:
    Allocations() {
      score = 0;
      jobs.clear();
    }
    void addJobResourcePair(JobTuplePtr j, ResourceList r) {
      jobs.push_back(std::make_pair(j,r));
    }

    float makespan() { float sum = 0; 
                       auto it = jobs.begin();
                       for(;it != jobs.end();it++) 
                       {
                          sum += std::get<0>(*std::get<0>(*it))->cost(*it);
                       } 
                       return sum;}


   size_t noJobs() { return jobs.size();}
   int getScore() { return score;}
   void setScore(int a ) { score  = a; }
   void serviceAllocations(Scheduler &s);
   void returnResources(Scheduler &s);


  private:
    JobResPairQ jobs;
    int score;
};

#endif
