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
  cout << "Client::send()" << endl;
  cout << "\tSending " << message->sizeBytes() << " bytes" << endl;
  ba::write(*socket_, ba::buffer((char *)message,
                                 message->sizeBytes())
            );
}

void Client::start() {
  cout << "Client::start()" << endl;
  cout << "\tHostname: " << name;
  cout << "\tPort: " << port << endl;
//  cout << "tcp::resolver()" << endl;
  try {
  tcp::resolver resolver(io_service);
//  cout << "tcp::resolver::query()" << endl;
  tcp::resolver::query q(name, boost::lexical_cast<string>(port));
//  cout << "resolver.resolve()" << endl;
  boost::system::error_code ec;
  tcp::resolver::iterator endpoint_it = resolver.resolve(q,ec);
  if (ec)
    cout << "cannot resolve address: " << ec.message() << endl;
//  cout << "tcp::socket()" << endl;
  socket_ = boost::make_shared<tcp::socket>(boost::ref(io_service));
//  cout << "asio::connect()" << endl;
  boost::asio::connect(*socket_, endpoint_it);
  } catch (std::exception& e) {
    cout << e.what() << endl;
  }

}

void Client::stop() {
  cout << "Client::stop()" << endl;

  // let the server know we're done
  msg_t msg;
  msg.msgId = MSG_DONE;
  msg.dataSize = 0;
  msg.paramsSize = 0;
  send(&msg);

  boost::system::error_code ignored_ec;
  socket_->shutdown(ba::ip::tcp::socket::shutdown_both, ignored_ec);
}

void Client::getResult(void* out, int sizeBytes) {
  cout << "Client::getResult()" << endl;
  int size = sizeBytes + sizeof(msg_t);
  char *buf = (char *)calloc(size, 1);
  ba::read(*socket_, boost::asio::buffer(buf, size));
  msg_t* rsp = (msg_t*)buf;
  if (sizeBytes != rsp->dataBytes()) {
    cerr << "\tError: reply size different than expected! ";
    cerr << "Expected " << sizeBytes << ", got " << rsp->dataBytes();
  }
  memcpy(out, rsp->data, rsp->dataBytes());
  free(buf);
}

int Client::read(char* buffer, size_t sizeBytes) {
  cout << "Client::read()" << endl;
  ba::read(*socket_, ba::buffer(buffer, sizeBytes));
  return 0;
}
