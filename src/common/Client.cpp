#include <Client.hpp>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost::asio::ip;
namespace ba = boost::asio;

void Client::send(msg_t *message) {
  char buf[1024];
  memcpy(buf, message, message->sizeBytes());
  boost::system::error_code error;
  socket_->write_some(boost::asio::buffer(buf), error);
}

void Client::start() {
  cout << "Starting Client..." << endl;
  cout << "Hostname: " << name << endl;
  cout << "Port: " << port << endl;

  boost::asio::io_service io_service;
  tcp::resolver resolver(io_service);
  tcp::resolver::query q(name, boost::lexical_cast<string>(port));
  tcp::resolver::iterator endpoint_it = resolver.resolve(q);
  socket_ = new tcp::socket(io_service);
  boost::asio::connect(*socket_, endpoint_it);
}

void Client::stop() {
  cout << "Client::shutting down..." << endl;

  // let the server know we're done
  msg_t msg;
  msg.msgId = MSG_DONE;
  msg.dataSize = 0;
  msg.paramsSize = 0;
  send(&msg);

  // XXX close socket?
  boost::system::error_code ignored_ec;
  socket_->shutdown(ba::ip::tcp::socket::shutdown_both, ignored_ec);
}

void Client::getResult(void* out) {
  // TODO[paul-g]: this is not safe, not some blocking IO
  // for (;;) {
  //   cout << "Waiting for result..." << endl;
    char buf[1024];
    boost::system::error_code error;
    size_t reply_len = socket_->read_some(boost::asio::buffer(buf), error);
    msg_t* rsp = (msg_t*)buf;
    memcpy(out, rsp->data, rsp->dataBytes());
    // if (error == boost::asio::error::eof)
    //   break; // Connection closed cleanly by peer.
    // else if (error)
    //   throw boost::system::system_error(error); // Some other error.
    //  }
}
