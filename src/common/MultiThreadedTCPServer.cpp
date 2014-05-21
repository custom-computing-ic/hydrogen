#include "MultiThreadedTCPServer.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>

#include "message.hpp"

using namespace std;
namespace ba = boost::asio;

MultiThreadedTCPServer::~MultiThreadedTCPServer() {}

MultiThreadedTCPServer::MultiThreadedTCPServer(const string& address, const string& port,
                                               size_t thread_pool_size)
  : thread_pool_size_(thread_pool_size),
    signals_(io_service_),
    acceptor_(io_service_),
    new_connection_()
{
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&MultiThreadedTCPServer::handle_stop, this));

  namespace bip = boost::asio::ip;
  bip::tcp::resolver resolver(io_service_);
  bip::tcp::resolver::query query(address, port);
  bip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(bip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
}
//Pretty sure we can replace this nasty vector with
//a nice thread_group
void MultiThreadedTCPServer::run() {
  cout << "Creating thread pool size " << thread_pool_size_ << endl;
  boost::thread_group worker_threads;

  //vector<boost::shared_ptr<boost::thread> > threads;
  for (size_t i = 0; i < thread_pool_size_; ++i) {

    //boost::shared_ptr<boost::thread> thread(new boost::thread(
      //                                                        boost::bind(&boost::asio::io_service::run, &io_service_)));
//    threads.push_back(thread);

    worker_threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));

  }

  //for (size_t i = 0; i < threads.size(); ++i)
  //  threads[i]->join();

  worker_threads.join_all();
}

void MultiThreadedTCPServer::start_accept() {
  cout << "Start accept\n";
  new_connection_.reset(new connection(io_service_, *this));
  acceptor_.async_accept(new_connection_->socket(),
                         boost::bind(&MultiThreadedTCPServer::handle_accept, this));
}

void MultiThreadedTCPServer::handle_accept() {
  cout << "Server:: handle_accept" << endl;
  new_connection_->start();
  start_accept();
}

void MultiThreadedTCPServer::handle_stop() {
  io_service_.stop();
}

connection::connection(ba::io_service& io_service,
                       MultiThreadedTCPServer& server)
  : strand_(io_service),
    socket_(io_service),
    server_(server)
{
}

ba::ip::tcp::socket& connection::socket() {
  return socket_;
}

void connection::start() {
  socket_.async_read_some(ba::buffer(buffer_),
                          strand_.wrap(
                                       boost::bind(&connection::handle_read, shared_from_this())));
}

void connection::handle_read() {

  // XXX TODO[paul-g]: Need to do this async
  msg_t* request = NULL;
  do {
    // unpack request
    request = (msg_t*)buffer_.data();

    // do work and generate reply
    msg_t* reply = server_.handle_request(request);

    // write reply back
    char buffer[reply->sizeBytes()];
    memcpy(buffer, reply, reply->sizeBytes());
    ba::write(socket_, ba::buffer(buffer, reply->sizeBytes()));

    // wait for new request
    // XXX this may not read all the bytes...
    socket_.read_some(ba::buffer(buffer_));
    // ba::read(socket_,
    // 	     ba::buffer(buffer_, 1024));
  } while (request != NULL && request->msgId != MSG_DONE);

  boost::system::error_code ignored_ec;
  socket_.shutdown(ba::ip::tcp::socket::shutdown_both, ignored_ec);
}

void connection::handle_write(const boost::system::error_code& e) {
  if (!e) {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(ba::ip::tcp::socket::shutdown_both, ignored_ec);
  }
}
