#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <message.hpp>
#include <string>
#include <boost/asio.hpp>
class Resource;
class Client {
private:
  int port;
  const std::string& name;
  boost::asio::io_service io_service;
  boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;

//  boost::asio::ip::tcp::resolver::query q;
//  boost::asio::ip::tcp::resolver::iterator endpoint_it; 

public:

  typedef Client super;

  /** Construct a client to connect to the given port of the given host */
  Client(int portNumber_, const std::string& name_) :
    name(name_),
    port(portNumber_)
  {}
  Client(Resource& r);

  virtual ~Client() {
//    delete socket_;
  }

  /** Send a message to target host. Must be connected. **/
  void send(msg_t *message);

  /** Read a message from socket into buff **/
  int read(char* buffer, size_t sizeBytes);

  /** Opens connection to target host **/
  virtual void start();

  /** Closes connection to taret host **/
  virtual void stop();

  virtual void getResult(void* out);
};

#endif /* _CLIENT_H_ */
