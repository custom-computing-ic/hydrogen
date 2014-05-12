#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <Server.hpp>
#include <string>

class Scheduler : public Server {

public:

  Scheduler(int port, const std::string& name) : super(port, name) {}

  virtual void handleRequest(msg_t& request);
};

#endif /* _SCHEDULER_H_ */
