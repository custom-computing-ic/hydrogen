#ifndef _SERVER_H_
#define _SERVER_H_

#include <message.hpp>
#include <string>

/** A general purpose server */
class Server {

  int portNumber, newsockfd, sockfd;
  const std::string &name;
  bool shuttingDown;
  void mainServerLoop();

public:
  typedef Server super;

  Server(int portNumber_, const std::string &name_)
      : portNumber(portNumber_), name(name_) {
    shuttingDown = false;
  }

  virtual ~Server() {}

  virtual void handleRequest(msg_t &request, msg_t &response) = 0;

  virtual void start();

  virtual void stop();
};

#endif /* _SERVER_H_ */
