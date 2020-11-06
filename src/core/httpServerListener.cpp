/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServerListener.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

Listener::Listener(net::io_context &ioContext_, ssl::context &sslContext_,
                   tcp::endpoint endpoint_,
                   std::shared_ptr<ServerState> serverState_,
                   std::shared_ptr<RouteImpl> routePtr_)
    : ioContext(ioContext_), sslContext(sslContext_), endpoint(endpoint_),
      serverState(serverState_), routePtr(routePtr_) {}

int Listener::run(net::yield_context yieldContext) {
  logStatus("::Listener::run", "START");
  beast::error_code ec;

  // Open the acceptor
  tcp::acceptor acceptor(ioContext);
  acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    logStatus("::Listener::run", "acceptor.open", ec);
    return -1;
  }

  // Allow address reuse
  acceptor.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    logStatus("::Listener::run", "acceptor.set_option", ec);
    return -1;
  }

  // Bind to the server address
  acceptor.bind(endpoint, ec);
  if (ec) {
    logStatus("::Listener::run", "acceptor.bind", ec);
    return -1;
  }

  // Start listening for connections
  acceptor.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    logStatus("::Listener::run", "acceptor.listen", ec);
    return -1;
  }

  for (;;) {
    tcp::socket socket(ioContext);
    acceptor.async_accept(socket, yieldContext[ec]);
    logStatus("::Listener::run", "acceptor.async_accept");
    if (ec) {
      logStatus("::Listener::run", "acceptor.async_accept", ec);
      continue;
    }

    std::shared_ptr<Connection> conn;
    {
      const std::lock_guard<std::mutex> lock(globalMutex);
      serverState->connectionList.emplace_back(std::make_shared<Connection>(
          std::move(socket), sslContext, serverState, routePtr));
      conn = serverState->connectionList.back();
    }
    net::spawn(acceptor.get_executor(),
               std::bind(&Connection::run, conn, std::placeholders::_1));
    logStatus("::Listener::run", "net::spawn");
  }
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
