#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <message.hpp>
#include <string>
#include <boost/asio.hpp>

class Client {

  const std::string& name;
  int port;

protected:
  boost::asio::ip::tcp::socket* socket_;

public:

  typedef Client super;

  /** Construct a client to connect to the given port of the given host */
  Client(int portNumber_, const std::string& name_) :
    port(portNumber_),
    name(name_)
  {}

  virtual ~Client() {
    delete socket_;
  }

  /** Send a message to target host. Must be connected. **/
  void send(msg_t *message);

  /** Read a message from socket into buff **/
  inline int read(char* buffer, int sizeBytes) {
    char buf[1024];
    memset(buf, 0, 1024);
    for (;;) {
      boost::system::error_code error;
      size_t reply_len = socket_->read_some(boost::asio::buffer(buf), error);
      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.
    }
    memcpy(buffer, buf, sizeBytes);
    // XXX error code...
    return 0;
  }

  /** Opens connection to target host **/
  virtual void start();

  /** Closes connection to taret host **/
  virtual void stop();

  virtual void getResult(void* out);
};

#endif /* _CLIENT_H_ */
