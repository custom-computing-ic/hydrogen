#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include <Client.hpp>
//typedef std::deque<Resource> ResourcePool;


class Resource : public Client {
  public:
    //TODO[mtottenh] : This is so bad.. Every time we copy a resouce
    // we end up invoking a new client connection!
    Resource(Resource const & r) :
      Client::super(r.getPort(),r.getName())
    {
        rid = r.getId();
    }
    Resource(int port,
             const std::string& name,
             int id) :
      Client::super(port,name)
    {
      rid = id;
      this->port = port;
      this->name = name;
    }
    ~Resource() {
    }
    virtual void start() { Client::start();};
    virtual void getResult(void *out) { Client::getResult(out); };
    virtual void stop() { Client::stop(); };
    virtual int getId() const { return rid; }
    virtual int getPort() const {return port;}
    virtual std::string getName() const  {return name;}



  private:
    int rid;
    float utilization;
    float idle_time;
    std::string name;
    int port;
};

#endif
