#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include <Server.hpp>
#include <string>

class Dispatcher : public Server {

public:

  Dispatcher(int port, const std::string &name) : super(port, name) {}

  virtual void handleRequest(msg_t& message, msg_t& response);

  void movingAverage();
};


#endif /* _DISPATCHER_H_ */
