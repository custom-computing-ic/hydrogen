#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include <Client.hpp>
#include <iostream>
#include <sstream>

class Resource {
  public:
    Resource(Resource const & r) 
    {
        rid = r.getId();
        port = r.getPort();
        name = r.getName();
        type = r.getType();
        /* should we copy idle/util time? */
    }

    Resource( int id, 
              int port_, 
              const std::string& name_, 
              const std::string& type_) 

        : rid (id),
          port (port_),
          name (name_),
          type (type_)
    {
      utilization = 0.0;
      idle_time = 0.0;
    }
   Resource ( const std::string& port_, const std::string& name_,
              const std::string& type_)
    : name(name_),
      type(type_)
    {
      rid = -1;
      std::stringstream ss;
      ss << port_;
      ss >> port;
    }
    virtual ~Resource() {
    }

    int getId() const { return rid; }
    int getPort() const {return port;}
    const std::string& getName() const  {return name;}
    const std::string& getType() const { return type;}
    friend std::ostream& operator<< (std::ostream& os, const Resource& r) {
      os << "{ID: " << r.rid << ",TYPE: " << r.type;
      os << ",HOSTNAME: " << r.name << ",PORT: " << r.port << "}";
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
    std::string type;
};

#endif
