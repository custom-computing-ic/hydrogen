#include <Job.hpp>

float defaultCostFunction(Job& j) {
  return j.getDefaultJobTime() /   j.noAllocdRes();
}

Job::Job(); 


Job::Job(int a, int b, std::function<float(Job&)> c) {
  min = a;
  max = b; 
  cost_func = c;
  issueTime = 0;
  dispatchTime = 0;
  defaultJobTime = 1.0;
  finishTime = 0;
} 
Job::Job(int a, int b, std::function<float(Job&)> c, float d) {
  min = a;
  max = b; 
  cost_func = c;
  issueTime = 0;
  dispatchTime = 0;
  defaultJobTime = d;
  finishTime = 0;
} 
/* for constructing a Job object from a message request */
Job::Job(msg_t& request) {
  //TODO[mtottenh]: Replace this with metadata gained from executor
  min = 1;
  max = 2; 
  //TODO[mtottenh]: Replace this with data from perf model
  cost_func = defaultCostFunction;
  //TODO[mtottenh]: Replace this with data from perf model
  defaultJobTime = 1.0;

  issueTime = 0;
  dispatchTime = 0;
  finishTime = 0;
  AllocatedResources = std::make_shared<ResourcePool>();
   
}
 

std::string Job::str() const {
  std::stringstream s; 
  s << "Job["<<this->getId() << "]"; 
  return s.str();
}
