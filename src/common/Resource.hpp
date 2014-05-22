#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include <Client.hpp>
#include <iostream>
//typedef std::deque<Resource> ResourcePool;


class Resource {
  public:
    Resource(Resource const & r) 
    {
        rid = r.getId();
        port = r.getPort();
        name = r.getName();
        /* should we copy idle/util time? */
    }

    Resource(int port_, const std::string& name_, int id) 
        : rid (id),
          port (port_),
          name (name_) 
    {
      utilization = 0.0;
      idle_time = 0.0;
    }

    virtual ~Resource() {
    }

    int getId() const { return rid; }
    int getPort() const {return port;}
    const std::string& getName() const  {return name;}
    friend std::ostream& operator<< (std::ostream& os, const Resource& r) {
      os << r.rid;
      return os;
    }
    friend bool operator==(const Resource& lhs, const Resource& rhs) {
      return lhs.rid == rhs.rid;
    }
    friend bool operator!=(const Resource& lhs, const Resource& rhs) {
      return !(lhs==rhs);
    }
  private:

    int rid;
    int port;
    float utilization;
    float idle_time;
    std::string name;
};

#endif
