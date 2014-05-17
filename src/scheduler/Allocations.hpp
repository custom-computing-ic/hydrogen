#ifndef __ALLOCATIONS_H_
#define __ALLOCATIONS_H_
#include <Job.hpp>
class Scheduler;

class Allocations {

  private:
    std::deque<Job> jobs;
    float score;
  public:
    Allocations() {
      score = 0;
    }

  /*  Allocations(const Allocations& a) {
      for ( auto& jobPtr : a.jobs) {
        this->jobs.push_back(  std::unique_ptr<Job>(new Job(*jobPtr)  ));
      }
      this->score = a.score;
    
    };*/
    /*
    Allocations& operator=(const Allocations& a) {
      //true deep coppy
      for ( auto& jobPtr : a.jobs) {
        this->jobs.push_back(  std::unique_ptr<Job>(new Job(*jobPtr)  ));
      }
      this->score = a.score;
      return *this;

    };
   ~Allocations() = default;

//    Allocations(Allocations&& a) : {
*/

  //  }

    void addJob(Job &j) {
      jobs.push_back(j);
    }

    float makespan() { float sum = 0; 
                       auto it = jobs.begin();
                       for(;it != jobs.end();it++) 
                       {
                          sum += it->cost();
                       } 
                       return sum;}
   int noJobs() { return jobs.size();}
   int getScore() { return score;}
   void setScore(int a ) { score  = a; }
   void serviceAllocations(Scheduler &s);
   void returnResources(Scheduler &s);
};

#endif
