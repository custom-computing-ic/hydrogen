#ifndef _DISPATCHERSERVER_H_
#define _DISPATCHERSERVER_H_

#include <MultiThreadedTCPServer.hpp>
#include <message.hpp>
#include <string>

class DispatcherServer : public MultiThreadedTCPServer {

public:

  DispatcherServer(const std::string& address,
                   const std::string& name,
                   std::size_t thread_pool_size) :
    super(address, name, thread_pool_size)
  {}

  void movingAverage_cpu(size_t n, size_t size, int *data, int *out);
  void movingAverage_dfe(int n, int size, int *data, int *out);

  msg_t* handle_request(msg_t* request);
};


#endif /* _DISPATCHERSERVER_H_ */
