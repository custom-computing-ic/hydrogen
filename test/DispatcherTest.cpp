#include <Dispatcher.hpp>
#include "message.hpp"

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <cstring>

using boost::asio::ip::tcp;
using namespace std;

int main() {
  msg_t* msg = NULL;
  int data_in[] = {1, 2, 3, 4};
  int exp[] = {2, 3, 0, 0};

  try {
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::iterator endpoint_it = resolver.resolve(tcp::resolver::query("localhost", "8112"));
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_it);
    for (;;) {
      char buf[1024];
      boost::system::error_code error;

      int sizeBytes = sizeof(msg_t) + 5 * sizeof(int);
      msg = (msg_t *)calloc(sizeBytes, 1);
      msg->msgId = MSG_MOVING_AVG;
      msg->dataSize = 4;
      msg->paramsSize = 1;
      memcpy(msg->data, data_in, sizeof(int) * msg->dataSize);
      *(msg->data + sizeof(int) * msg->dataSize) = 3;

      memcpy(buf, msg, sizeBytes);

      socket.write_some(boost::asio::buffer(buf), error);

      char buf2[1024];
      size_t reply_len = socket.read_some(boost::asio::buffer(buf2), error);

      msg = (msg_t*)buf2;

      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

    }
  } catch (exception& e) {
    cerr << e.what() << endl;
  }

  msg->print();

  return msg && (memcmp(exp, msg->data, 4 * sizeof(int)) == 0) ? 0 : 1;
}
