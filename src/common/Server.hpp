#ifndef _SERVER_H_
#define _SERVER_H_

#include <message.hpp>

/** A general purpose server */
class Server {

  int portNumber;

public:

  Server(int portNumber) {
    this->portNumber = portNumber;
  }

  virtual ~Server() {
  }

  virtual void handleRequest(msg_t request);

  virtual void start();
};

#endif /* _SERVER_H_ */
