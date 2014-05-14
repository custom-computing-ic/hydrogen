#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include <Client.hpp>
//typedef std::deque<Resource> ResourcePool;


class Resource : public Client {
  public:
    Resource(int port,
             const std::string& name,
             int id) :
      Client::super(port,name),
      rid(id)
    {

    }
    
    virtual void start();

    virtual void stop();





  private:
    int rid;
    float utilization;
    float idle_time;
};

#endif
