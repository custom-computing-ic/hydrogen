#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <message.hpp>
#include <string>

class Client {

  int sockfd, portNumber;
  const std::string& name;

public:
  Client(int portNumber_, const std::string& name_) :
    portNumber(portNumber_),
    name(name_)
  {}

  virtual ~Client() {}

  /** Send a message to target host. Must be connected. **/
  void send(msg_t *message, int sizeBytes);

  /** Closes connection to taret host **/
  void close();

  /** Opens connection to target host **/
  void start();
};

#endif /* _CLIENT_H_ */
