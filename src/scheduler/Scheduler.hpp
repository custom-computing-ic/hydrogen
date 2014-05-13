#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <Server.hpp>
#include <Client.hpp>
#include <string>
#include <memory>
#include <deque>
/** The scheduler is a server for the client API and a client of the dispatcher **/
class Scheduler : public Server {
  typedef std::deque< std::shared_ptr<Client> > ResourcePool;
  //TODO replace int with jobtype.. 
  typedef std::deque< std::shared_ptr<int> > JobQueue;
public:

  Scheduler(int port,
	    const std::string& name,
	    int dispatcherPortNumber,
	    const std::string& dispatcherHostname) :
    Server::super(port, name)
  {
    this->resPool = std::unique_ptr<ResourcePool>(new ResourcePool());
    this->readyQ = std::unique_ptr<JobQueue>(new JobQueue()); 
    this->runQ = std::unique_ptr<JobQueue>(new JobQueue()); 
    this->finishedJobs = std::unique_ptr<JobQueue>(new JobQueue()); 
    this->addResource(dispatcherPortNumber,dispatcherHostname);
  }

  virtual void handleRequest(msg_t& request, msg_t& response);
  virtual void addToReadyQ(msg& request, msg_t& response);
  virtual void start();

  virtual void stop();

private:

  void addToReadyQ(msg& request);

  inline void addResource(int PortNo, const std::string& Hostname) {
        resPool->push_back(std::make_shared<Client>(PortNo,Hostname));
  };
   
  std::unique_ptr<ResourcePool> resPool; 
  std::unique_ptr<JobQueue> readyQ;
  std::unique_ptr<JobQueue> runQ;
  std::unique_ptr<JobQueue> finishedJobs;
/*  std::deque<> readyQ
  std::deque<> runQ
  std::deque<> finishedJobs
*/

};

#endif /* _SCHEDULER_H_ */
