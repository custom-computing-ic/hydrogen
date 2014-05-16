#ifndef _DISPATCHERSERVER_H_
#define _DISPATCHERSERVER_H_

#include <MultiThreadedTCPServer.hpp>
#include <string>

class DispatcherServer : public MultiThreadedTCPServer {

public:

  DispatcherServer(const std::string& address,
		   const std::string& name,
		   std::size_t thread_pool_size) :
    super(address, name, thread_pool_size)
  {}

  std::string do_work() {
    std::cout << "Dispatcher:: do work" << std::endl;
    return "Result";
  }

};


#endif /* _DISPATCHERSERVER_H_ */
