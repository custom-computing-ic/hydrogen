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
    for (auto it : jobs) {
      sum += std::get<0>(*std::get<0>(*it))->cost(*it);
    }
    return sum;
  }

  float totalPriorities() { return 1.0; }

  size_t noJobs() {
    size_t count = 0;
    JobTuplePtr j;
    ResourceListPtr rl;
    for (auto it : jobs) {
      std::tie(j, rl) = *it;
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
    double jloMetricNeg = 0;
    double jloMetricPos = 0;
    for (auto it : jobs) {
      JobTuplePtr p = std::get<0>(*it);
      ResourceListPtr rp = std::get<1>(*it);
      LOG(debug) << rp->size();
      int resourceCount = rp->size();
      Job j = *(std::get<0>(*p));
      double jloMetric = j.getJlo(resourceCount);
      if (jloMetric < 0)
	jloMetricNeg += jloMetric;
      else
	jloMetricPos += jloMetric;
    }

    if (jloMetricNeg < 0)
      return jloMetricNeg;

    return jloMetricPos;
  }

private:
  JobResPairQ jobs;
  int score;
};

#endif
