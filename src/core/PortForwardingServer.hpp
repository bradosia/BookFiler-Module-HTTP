/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_PORT_FORWARDING_SERVER_H
#define BOOKFILER_MODULE_PORT_FORWARDING_SERVER_H

// C++11
#include <functional>
#include <iostream>
#include <memory>
#include <queue>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/reader.h> // rapidjson::ParseResult
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/asio.hpp>
#include <boost/config.hpp>

// Local Project
#include "PortForwardingServerListener.hpp"

#define BIND_HANDLER(handler)                                                  \
  ([this, self = shared_from_this()](boost::system::error_code ec) { handler(ec); })
#define BIND_HANDLER_S(handler)                                                \
  ([this, self = shared_from_this()](boost::system::error_code ec, std::size_t length) {      \
    handler(ec, length);                                                       \
  })

/*
 * bookfiler - port
 * Port fowarding utilities. Initially this utility was made to forward mysql
 * traffic through a secure ssh connection.
 */
namespace bookfiler {
namespace port {

class ListenerImpl {};

class ForwardingServerImpl {
private:
  std::shared_ptr<rapidjson::Value> settingsDoc;
  std::shared_ptr<ServerState> serverState;
  std::shared_ptr<boost::asio::io_context> ioContext;
  std::shared_ptr<ListenerImpl> listener;
  std::vector<std::thread> threadList;

  boost::asio::ip::tcp::socket src_socket, dst_socket;
  static const std::size_t read_buffer_length = 1500;
  char src_read_buffer[read_buffer_length], dst_read_buffer[read_buffer_length];
  std::size_t write_buffer_length = read_buffer_length;
  typedef std::pair<void *, size_t> write_buffer_type;
  std::queue<write_buffer_type> src_write_queue, dst_write_queue;

  void dst_connect_handler(boost::system::error_code ec);
  void src_read_some_handler(boost::system::error_code ec, std::size_t length);
  void dst_read_some_handler(boost::system::error_code ec, std::size_t length);
  void src_write_handler(boost::system::error_code ec, std::size_t);
  void dst_write_handler(boost::system::error_code ec, std::size_t);
  void enqueue_src_write(std::size_t length);
  void enqueue_dst_write(std::size_t length);

public:

  void start(boost::asio::ip::tcp::endpoint remote) {
    dst_socket.async_connect(remote, BIND_HANDLER(dst_connect_handler));
  }

  ForwardingServerImpl(
      std::map<std::string, bookfiler::port::newForwardingServerType> map_);
  ~ForwardingServerImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value>);
  int run();
  void runIoContext();
};

class server {
public:
  server(boost::asio::io_service &io_service, short port)
      : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
        socket_(io_service) {
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
      if (!ec) {
        auto s = std::make_shared<ServerImpl>(std::move(socket_));
        s->start(boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address_v4::from_string("202.202.43.42"), 80));
      }
      do_accept();
    });
  }

  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::socket socket_;
};

int main() {
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::socket socket(io_service);
  // socket.connect()
  server s(io_service, 2333);
  io_service.run();
  return 0;
}

} // namespace port
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_PORT_FORWARDING_SERVER_Hs
