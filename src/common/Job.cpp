#include <Job.hpp>

//static int jobID = 0;
//int getNextId() {return ++jobID;}

float defaultCostFunction(Job& j) {
  return j.getDefaultJobTime() /   j.noAllocdRes();
}



Job::Job(int a, int b, std::function<float(Job&)> c) {
  min = a;
  max = b;
  cost_func = c;
  issueTime = 0;
  dispatchTime = 0;
  defaultJobTime = 1.0;
  finishTime = 0;
  status = -1;
}
Job::Job(int a, int b, std::function<float(Job&)> c, float d) {
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
  AllocatedResources = std::make_shared<ResourcePool>();
  status = 2;
  req = request;
  jid = id;
}
// TODO[mtottenh]: Change this... I dont think it needs to return a result anymore
msg_t Job::run() {
  AllocatedResources->front()->send(&req);
  return req;
}


std::string Job::str() const {
  std::stringstream s;
  s << "Job["<<this->getId() << "]";
  return s.str();
}

void Job::getResponse(char* buffer, int sizeBytes) {
  int n = AllocatedResources->front()->read(buffer, sizeBytes);
}
