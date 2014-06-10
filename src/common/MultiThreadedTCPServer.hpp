#ifndef _MULTITHREADEDTCPSERVER_H_
#define _MULTITHREADEDTCPSERVER_H_

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>

#include "message.hpp"

class MultiThreadedTCPServer;

/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
  /// Construct a connection with the given io_service.
  explicit connection(boost::asio::io_service& io_service,
		      MultiThreadedTCPServer& server);

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

private:
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e,
		   std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e);

  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// Buffer for incoming data.
  boost::array<char, 192 * 1024 * 1024 > buffer_;

  MultiThreadedTCPServer& server_;
};

typedef boost::shared_ptr<connection> connection_ptr;

class MultiThreadedTCPServer
  : private boost::noncopyable
{
public:
  explicit MultiThreadedTCPServer(const std::string& address, const std::string& port,
      std::size_t thread_pool_size);

  virtual ~MultiThreadedTCPServer();

  typedef MultiThreadedTCPServer super;

  void run();

  // A thread safe function to be called by each connection
  virtual msg_t* handle_request(msg_t* request)=0;


private:
  void start_accept();

  void handle_accept();

  // Stop the server.
  void handle_stop();


  std::size_t thread_pool_size_;

  boost::asio::io_service io_service_;

  boost::asio::signal_set signals_;

  boost::asio::ip::tcp::acceptor acceptor_;
  
  boost::thread_group worker_threads;

  connection_ptr new_connection_;
};
#endif /* _MULTITHREADEDTCPSERVER_H_ */
