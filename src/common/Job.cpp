#include <Job.hpp>


float defaultCostFunction(JobResPair& p) {
  return std::get<0>(*std::get<0>(p))->getDefaultJobTime() /   std::get<1>(p).size();
}



Job::Job(int a, int b, CostFunctionType c) {
  min = a;
  max = b;
  cost_func = c;
  issueTime = 0;
  dispatchTime = 0;
  defaultJobTime = 1.0;
  finishTime = 0;
  status = -1;
}
Job::Job(int a, int b, CostFunctionType c, float d) {
  min = a;
  max = b;
  cost_func = c;
  issueTime = 0;
  dispatchTime = 0;
  defaultJobTime = d;
  finishTime = 0;
  status = -1;
}
/* for constructing a Job object from a message request */
Job::Job(msg_t& request,int id) {
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
  status = 2;
  req = request;
  jid = id;
}
// TODO[mtottenh]: Change this... I dont think it needs to return a result anymore
msg_t Job::run() {
  std::cout << "In Job::run() : Needs Implementing\n";
  return req;
}


std::string Job::str() const {
  std::stringstream s;
  s << "Job["<<this->getId() << "]";
  return s.str();
}

void Job::getResponse(char* buffer, size_t sizeBytes) {
  std::cout << "In Job::getResponse: Needs Implementing\n";
}
