/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServerSession.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

Session::Session(tcp::socket &&socket, ssl::context &sslContext,
                 std::shared_ptr<std::string const> const &docRoot_)
    : sslStream(std::move(socket), sslContext), docRoot(docRoot_) {
  req = std::make_shared<http::request<http::string_body>>();
}

// Start the asynchronous operation
void Session::run() {
  // We need to be executing within a strand to perform async operations
  // on the I/O objects in this session. Although not strictly necessary
  // for single-threaded contexts, this example code is written to be
  // thread-safe by default.
  net::dispatch(
      sslStream.get_executor(),
      beast::bind_front_handler(&Session::on_run, shared_from_this()));
}

void Session::on_run() {
  // Set the timeout.
  beast::get_lowest_layer(sslStream).expires_after(std::chrono::seconds(30));

  // Perform the SSL handshake
  sslStream.async_handshake(
      ssl::stream_base::server,
      beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
}

void Session::on_handshake(beast::error_code ec) {
  if (ec)
    return fail(ec, "handshake");

  do_read();
}

void Session::do_read() {
  // Make the request empty before reading,
  // otherwise the operation behavior is undefined.
  // req_ = {};
  req = std::make_shared<http::request<http::string_body>>();

  // Set the timeout.
  beast::get_lowest_layer(sslStream).expires_after(std::chrono::seconds(30));

  // Read a request
  http::async_read(
      sslStream, buffer, *req,
      beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void Session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  // This means they closed the connection
  if (ec == http::error::end_of_stream)
    return do_close();

  if (ec)
    return fail(ec, "read");

  // Send the response
  res = std::make_shared<http::response<http::string_body>>();
  handleRequest(sessionDocument, req, res, routeSignal);
  // this lines 1,152KB -> 3,887KB
  //http::write(sslStream, *res, ec);
  //on_write(true, ec, 0);
  http::async_write(sslStream, *res,
                    beast::bind_front_handler(&Session::on_write,
                                              shared_from_this(),
                                              res->need_eof()));
}

void Session::on_write(bool close, beast::error_code ec,
                       std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return fail(ec, "write");

  if (close) {
    // This means we should close the connection, usually because
    // the response indicated the "Connection: close" semantic.
    return do_close();
  }

  // We're done with the response so delete it
  res = nullptr;

  // Read another request
  do_read();
}

void Session::do_close() {
  // Set the timeout.
  beast::get_lowest_layer(sslStream).expires_after(std::chrono::seconds(30));

  // Perform the SSL shutdown
  sslStream.async_shutdown(
      beast::bind_front_handler(&Session::on_shutdown, shared_from_this()));
}

void Session::on_shutdown(beast::error_code ec) {
  if (ec)
    return fail(ec, "shutdown");

  // At this point the connection is closed gracefully
}

int Session::setRouteSignal(std::shared_ptr<routeSignalType> routeSignal_) {
  routeSignal = routeSignal_;
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
