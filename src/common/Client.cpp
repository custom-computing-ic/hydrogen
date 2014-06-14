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
  if (message->msgId != MSG_DONE)
    cout << "(DEBUG):\t\t* Sending Request" << endl;
  size_t byteSize= message->totalBytes;
  float kb = (float)byteSize / 1024;
  float mb = kb / 1024;
  cout << "(DEBUG):\t\t\t->Sending ";
  if ( mb >= 1) {
    cout << mb << " MB" << endl;
  } else {
    if ( kb >= 1) {
     cout << kb << " KB" << endl;
    } else {
     cout << byteSize << " Bytes" << endl;
    }
  }
  ba::write(*socket_,
            ba::buffer((char *)message, message->totalBytes)
           );
}

void Client::start() {
  cout << "(DEBUG):\t\t* Client::start()" << endl;
  cout << "(DEBUG): \tHostname: " << name;
  cout << "\tPort: " << port << endl;
  try {
  tcp::resolver resolver(io_service);
//  cout << "tcp::resolver::query()" << endl;
  tcp::resolver::query q(name, boost::lexical_cast<string>(port));
//  cout << "resolver.resolve()" << endl;
  boost::system::error_code ec;
  tcp::resolver::iterator endpoint_it = resolver.resolve(q,ec);
  if (ec)
    cout << "(ERROR): cannot resolve address: " << ec.message() << endl;
  socket_ = boost::make_shared<tcp::socket>(boost::ref(io_service));
//  cout << "(DEBUG): asio::connect()" << endl;
  boost::asio::connect(*socket_, endpoint_it);
  } catch (std::exception& e) {
    cout << "(ERROR): Client::start() - " << e.what() << endl;
  }

}

void Client::stop() {
  cout << "(DEBUG):\t\t* Closing connection" << endl;

  // let the server know we're done
  msg_t* msg = msg_done();
  send(msg);
  free(msg);

  boost::system::error_code ignored_ec;
  socket_->shutdown(ba::ip::tcp::socket::shutdown_both, ignored_ec);
}

void Client::getResult(void* out, int sizeBytes) {
  cout << "(DEBUG):\t\t* Waiting for result..." << endl;
  int size = sizeBytes + sizeof(msg_t);
  char *buf = (char *)calloc(size, 1);


//  try {
    ba::read(*socket_, boost::asio::buffer(buf, size));
//  } catch (std::exception& e) {
//    std::cout << "(ERROR):\t\t* Failed to read from socket."
//              << "Check if remote host crashed\n";
//  }



  msg_t* rsp = (msg_t*)buf;
  if (sizeBytes != rsp->dataBytes) {
    cerr << "(ERROR):\t\t* Error: reply size different than expected! ";
    cerr << "Expected " << sizeBytes << ", got " << rsp->dataBytes;
  }
  memcpy(out, rsp->data, rsp->dataBytes);
  free(buf);
}

int Client::read(char* buffer, size_t sizeBytes) {
//  cout << "(DEBUG): Client::read()" << endl;
//  try {
    ba::read(*socket_, ba::buffer(buffer, sizeBytes));
//  } catch (std::exception& e) {
//    std::cout << "(ERROR):\t\t* Failed to read from socket."
//              << "Check if remote host crashed\n";
//    return 1;
//  }
  return 0;
}
