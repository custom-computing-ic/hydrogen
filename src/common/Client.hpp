#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <message.hpp>
#include <string>

class Client {

  int sockfd, portNumber;
  const std::string& name;

public:

  typedef Client super;

  /** Construct a client to connect to the given port of the given host */
  Client(int portNumber_, const std::string& name_) :
    portNumber(portNumber_),
    name(name_)
  {}

  virtual ~Client() {}

  /** Send a message to target host. Must be connected. **/
  void send(msg_t *message, int sizeBytes);

  /** Opens connection to target host **/
  virtual void start();

  /** Closes connection to taret host **/
  virtual void stop();

  virtual void getResult(void* out);
};

#endif /* _CLIENT_H_ */
