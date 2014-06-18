#ifndef __ALLOCATIONS_H_
#define __ALLOCATIONS_H_
#include <Job.hpp>
#include <typedefs.hpp>
#include <Logging.hpp>
class Allocations {

public:
  Allocations() {
    score = 0;
    jobs.clear();
  }
  void addJobResourcePair(JobTuplePtr j, ResourceListPtr r) {
    jobs.push_back(std::make_shared<JobResPair>(std::make_pair(j, r)));
  }

  float makespan() {
    float sum = 0;
    JobResPairQ::iterator it = jobs.begin();
    for (; it != jobs.end(); it++) {
      sum += std::get<0>(*std::get<0>(**it))->cost(**it);
    }
    return sum;
  }

  float totalPriorities() { return 1.0; }

  size_t noJobs() {
    size_t count = 0;
    JobTuplePtr j;
    ResourceListPtr rl;
    JobResPairQ::iterator it = jobs.begin();
    for (; it != jobs.end(); it++) {
      std::tie(j, rl) = **it;
      if (rl != nullptr && rl->size() > 0) {
        count++;
      }
    }
    return count;
  }

  int getScore() const { return score; }

  void setScore(int a) { score = a; }

  void serviceAllocations(Scheduler &s);

  void returnResources(Scheduler &s);

  double getJLOMetric() {
    LOGF(debug, "Computing jloMetric for %1% jobs") % noJobs();
    JobResPairQ::iterator it = jobs.begin();
    for ( ; it != jobs.end(); it++) {
      // JobTuplePtr p = std::get<0>(**it);
      // Job j = *(std::get<0>(*p));
    }
    return 0;
  }

private:
  JobResPairQ jobs;
  int score;
};

#endif
