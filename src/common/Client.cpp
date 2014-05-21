#include <Client.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <Resource.hpp>
using namespace std;
using namespace boost::asio::ip;
namespace ba = boost::asio;
Client::Client(Resource &r) :
  name(std::string(r.getName().c_str()))
{
  port = r.getPort();
}

void Client::send(msg_t *message) {
  ba::write(*socket_, ba::buffer((char *)message,
                                 message->sizeBytes())
            );
}

void Client::start() {
  cout << "Starting Client..." << endl;
  cout << "Hostname: " << name << endl;
  cout << "Port: " << port << endl;

  tcp::resolver resolver(io_service);
  tcp::resolver::query q(name, boost::lexical_cast<string>(port));
  tcp::resolver::iterator endpoint_it = resolver.resolve(q);
  socket_ = boost::make_shared<tcp::socket>(boost::ref(io_service));
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

void Client::getResult(void* out, int sizeBytes) {
  // TODO[paul-g]: this is not safe, not some blocking IO
  // for (;;) {
  //   cout << "Waiting for result..." << endl;
  int size = sizeBytes + sizeof(msg_t);
  char buf[size];
  ba::read(*socket_, boost::asio::buffer(buf, size));
  msg_t* rsp = (msg_t*)buf;

  if (sizeBytes != rsp->dataBytes()) {
    cerr << "Error: reply size different than expected! ";
    cerr << "Expected " << sizeBytes << ", got " << rsp->dataBytes();
  }

  memcpy(out, rsp->data, rsp->dataBytes());
  // if (error == boost::asio::error::eof)
  //   break; // Connection closed cleanly by peer.
  // else if (error)
  //   throw boost::system::system_error(error); // Some other error.
  //  }
}

int Client::read(char* buffer, size_t sizeBytes) {
  ba::read(*socket_, ba::buffer(buffer, sizeBytes));
  return 0;
}
