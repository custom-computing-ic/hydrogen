#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include <Client.hpp>
//typedef std::deque<Resource> ResourcePool;


class Resource {
  public:
    //TODO[mtottenh] : This is so bad.. Every time we copy a resouce
    // we end up invoking a new client connection!
    Resource(Resource const & r) 
    {
        rid = r.getId();
        port = r.getPort();
        name = r.getName();
        /* should we copy idle/util time? */
    }

    Resource(int port_, const std::string& name_, int id) 
        : name (name_),
          rid (id),
          port (port_)
    {
      rid = id;
    }

    virtual ~Resource() {
    }

//    virtual void getResult(void *out);
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
