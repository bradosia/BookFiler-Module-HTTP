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
                   tcp::endpoint endpoint,
                   std::shared_ptr<std::string const> const &docRoot_)
    : ioContext(ioContext_), sslContext(sslContext_), acceptor(ioContext_),
      docRoot(docRoot_) {
  beast::error_code ec;

  // Open the acceptor
  acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
    return;
  }

  // Allow address reuse
  acceptor.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    fail(ec, "set_option");
    return;
  }

  // Bind to the server address
  acceptor.bind(endpoint, ec);
  if (ec) {
    fail(ec, "bind");
    return;
  }

  // Start listening for connections
  acceptor.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
    return;
  }
}

void Listener::run() { do_accept(); }

void Listener::do_accept() {
  // The new connection gets its own strand
  acceptor.async_accept(
      net::make_strand(ioContext),
      beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
}

void Listener::on_accept(beast::error_code ec, tcp::socket socket) {
  if (ec) {
    fail(ec, "accept");
  } else {
    // Create the session and run it
    session = std::make_shared<Session>(std::move(socket), sslContext, docRoot);
    session->setRouteSignal(routeSignal);
    session->run();
  }

  // Accept another connection
  do_accept();
}

int Listener::setRouteSignal(std::shared_ptr<routeSignalType> routeSignal_) {
  routeSignal = routeSignal_;
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
