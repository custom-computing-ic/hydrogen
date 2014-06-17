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

#include <Logging.hpp>

using namespace std;
using namespace boost::asio::ip;
namespace ba = boost::asio;
Client::Client(Resource &r) : name(std::string(r.getName().c_str())) {
  port = r.getPort();
}

void Client::send(msg_t *message) {
  if (message->msgId != MSG_DONE)
    LOG(debug) << "\t\t* Sending Request" ;
  size_t byteSize = message->totalBytes;
  float kb = (float)byteSize / 1024;
  float mb = kb / 1024;
  std::stringstream ss;
  ss << "\t\t\t->Sending ";
  if (mb >= 1) {
    ss << mb << " MB" ;
  } else  if (kb >= 1) {
    ss << kb << " KB" ;
  } else {
    ss << byteSize << " Bytes" ;
  }
  LOG(debug) << ss.str();
  ba::write(*socket_, ba::buffer((char *)message, message->totalBytes));
}

void Client::start() {
  LOGF(debug, "\tConnecting to %1%:%2%") % name % port;
  try {
    tcp::resolver resolver(io_service);
    tcp::resolver::query q(name, boost::lexical_cast<string>(port));
    boost::system::error_code ec;
    tcp::resolver::iterator endpoint_it = resolver.resolve(q, ec);
    if (ec)
      LOG(error) << "(ERROR): cannot resolve address: " << ec.message() ;
    socket_ = boost::make_shared<tcp::socket>(boost::ref(io_service));
    boost::asio::connect(*socket_, endpoint_it);
  }
  catch (std::exception &e) {
    LOG(error) << "(ERROR): Client::start() - " << e.what() ;
  }
}

void Client::stop() {
  LOG(debug) << "\t\t* Closing connection" ;

  // let the server know we're done
  msg_t *msg = msg_done();
  send(msg);
  free(msg);

  boost::system::error_code ignored_ec;
  socket_->shutdown(ba::ip::tcp::socket::shutdown_both, ignored_ec);
}

void Client::getResult(void *out, int sizeBytes) {
  LOG(debug) << "\t\t* Waiting for result..." ;
  int size = sizeBytes + sizeof(msg_t);
  char *buf = (char *)calloc(size, 1);
  ba::read(*socket_, boost::asio::buffer(buf, size));
  msg_t *rsp = (msg_t *)buf;
  if (sizeBytes != rsp->dataBytes) {
    LOG(error) << "\t\t* Error: reply size different than expected! "
	       << "Expected " << sizeBytes << ", got " << rsp->dataBytes;
  }
  memcpy(out, rsp->data, rsp->dataBytes);
  free(buf);
}

int Client::read(char *buffer, size_t sizeBytes) {
  ba::read(*socket_, ba::buffer(buffer, sizeBytes));
  return 0;
}
