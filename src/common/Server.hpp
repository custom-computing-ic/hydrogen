#ifndef _SERVER_H_
#define _SERVER_H_

#include <message.hpp>
#include <string>

/** A general purpose server */
class Server {

  int portNumber;
  const std::string& name;

  void mainServerLoop(int sockfd);

public:

  typedef Server super;

  Server(int portNumber_, const std::string& name_) :
    portNumber(portNumber_),
    name(name_)
  {}

  virtual ~Server() {
  }

  virtual void handleRequest(msg_t& request)=0;

  virtual void start();
};

#endif /* _SERVER_H_ */
