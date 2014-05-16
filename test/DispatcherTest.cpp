#include <Dispatcher.hpp>
#include "message.hpp"

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

int main() {
  try {
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::iterator endpoint_it = resolver.resolve(tcp::resolver::query("localhost", "8111"));
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_it);
    for (;;) {
      char buf[1024];
      boost::system::error_code error;

      int sizeBytes = sizeof(msg_t) + 2 * sizeof(int);
      msg_t *msg = (msg_t *)calloc(sizeBytes, 1);
      msg->msgId = MSG_MOVING_AVG;
      msg->dataSize = 1;
      msg->paramsSize = 1;
      *(msg->data) = 13;
      *(msg->data + sizeof(int)) = 2;

      memcpy(buf, msg, sizeBytes);

      socket.write_some(boost::asio::buffer(buf), error);
      size_t reply_len = socket.read_some(boost::asio::buffer(buf), error);

      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      cout.write(buf, reply_len);
    }
  } catch (exception& e) {
    cerr << e.what() << endl;
  }
}
