#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <Server.hpp>
#include <Client.hpp>
#include <string>

/** The scheduler is a server for the client API and a client of the dispatcher **/
class Scheduler : public Server, public Client {

public:

  Scheduler(int port,
	    const std::string& name,
	    int dispatcherPortNumber,
	    const std::string& dispatcherHostname) :
    Server::super(port, name),
    Client::super(dispatcherPortNumber, dispatcherHostname)
  {}

  virtual void handleRequest(msg_t& request, msg_t& response);
  virtual void addToReadyQ(msg& request, msg_t& response);
  virtual void start();

  virtual void stop();
};

#endif /* _SCHEDULER_H_ */
