#include <Job.hpp>

float defaultCostFunction(JobResPair &p) {
  return std::get<0>(*std::get<0>(p))->getDefaultJobTime() /
         std::get<1>(p)->size();
}

/* for constructing a Job object from a message request */
Job::Job(msg_t *request, int id) : req(request) {
  // TODO[mtottenh]: Replace this with metadata gained from executor
  min = 1;
  max = 4;
  // TODO[mtottenh]: Replace this with data from perf model
  cost_func = defaultCostFunction;
  // TODO[mtottenh]: Replace this with data from perf model
  defaultJobTime = (float)request->predicted_time;
  // TODO[mtottenh]: This works for now, but this needs to
  // change for after the demmo.
  priority = request->clientId;
  issueTime = boost::chrono::system_clock::now();
  dispatchTime = boost::chrono::system_clock::now();
  finishTime = boost::chrono::system_clock::now();
  status = 2;
  //  realloc(&(req.data),request.dataBytes() * sizeof(char));
  //  memcpy(req.data,request.data,request.dataBytes());
  //  req = request;
  jid = id;
  rsp = nullptr;
}
// TODO[mtottenh]: Change this... I dont think it needs to return a result
// anymore
msg_t Job::run() {
  std::cout << "In Job::run() : Needs Implementing\n";
  return *req;
}

std::string Job::str() const {
  std::stringstream s;
  s << "Job[" << this->getId() << "]";
  return s.str();
}

void Job::getResponse(char *buffer, size_t sizeBytes) {
  std::cout << "In Job::getResponse: Needs Implementing";
  std::cout << buffer[0] << sizeBytes << "\n";
}
