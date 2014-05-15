#include "MultiThreadedTCPServer.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>

using namespace std;
namespace ba = boost::asio;

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

void MultiThreadedTCPServer::run() {
  cout << "Creating thread pool size " << thread_pool_size_ << endl;
  vector<boost::shared_ptr<boost::thread> > threads;
  for (size_t i = 0; i < thread_pool_size_; ++i) {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
  }

  for (size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
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
  string reply = server_.do_work();
  ba::async_write(socket_,
                           ba::buffer(reply),
                           strand_.wrap(
                                        boost::bind(&connection::handle_write, shared_from_this(),
                                                    ba::placeholders::error)));
}

void connection::handle_write(const boost::system::error_code& e) {
  if (!e) {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(ba::ip::tcp::socket::shutdown_both, ignored_ec);
  }
}
