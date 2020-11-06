/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "ServerConnection.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

Connection::Connection(tcp::socket socket_, ssl::context &sslContext_,
                       std::shared_ptr<ServerState> serverState_,
                       std::shared_ptr<RouteImpl> routePtr_)
    : sslStream(beast::ssl_stream<beast::tcp_stream>(std::move(socket_),
                                                     sslContext_)),
      serverState(serverState_), routePtr(routePtr_) {}
Connection::~Connection() {}

int Connection::run(net::yield_context yieldContext) {
  logStatus("::ServerImpl::do_session", "START");
  bool close = true;
  beast::error_code ec;

  // Set the timeout.
  beast::get_lowest_layer(sslStream).expires_after(std::chrono::seconds(30));

  // Perform the SSL handshake
  sslStream.async_handshake(ssl::stream_base::server, yieldContext[ec]);
  if (ec) {
    logStatus("::Connection::run", "sslStream.async_handshake", ec);
    return -1;
  }

  // This buffer is required to persist across reads
  beast::flat_buffer buffer;

  while (close) {
    int rc = 0;
    // Set the timeout.
    beast::get_lowest_layer(sslStream).expires_after(std::chrono::seconds(30));

    // Read a request
    requestBeastInternal reqBeast =
        std::make_shared<http::request<http::string_body>>();
    http::async_read(sslStream, buffer, *reqBeast, yieldContext[ec]);
    std::stringstream ss;
    ss << "http::async_read request\n" << *reqBeast << "\n";
    logStatus("::ServerImpl::do_session", ss.str());

    if (ec == http::error::end_of_stream) {
      logStatus("::Connection::run", "http::error::end_of_stream");
      break;
    }
    if (ec) {
      logStatus("::Connection::run", "http::async_read", ec);
      return -1;
    }

    // Send the response
    responseBeastInternal resBeast =
        std::make_shared<http::response<http::string_body>>();
    // Make sure we can handle the method
    if (reqBeast->method() != http::verb::get &&
        reqBeast->method() != http::verb::head) {
      return badRequest(reqBeast, resBeast, "Unknown HTTP-method");
    }
    // Request path must be absolute and not contain "..".
    if (reqBeast->target().empty() || reqBeast->target()[0] != '/' ||
        reqBeast->target().find("..") != beast::string_view::npos) {
      return badRequest(reqBeast, resBeast, "Illegal request-target");
    }
    // Setup response and requests
    logStatus("::Connection::do_read", "setup phase");
    std::shared_ptr<RequestImpl> reqImpl = std::make_shared<RequestImpl>();
    reqImpl->setRequest(reqBeast);
    reqImpl->parseRequest();
    std::shared_ptr<ResponseImpl> resImpl = std::make_shared<ResponseImpl>();
    resImpl->setResponse(resBeast);
    rc = routePtr->doGetSignal(reqImpl->path(), reqImpl, resImpl);
    if (rc < 0) {
      notFound(reqBeast, resBeast, reqImpl->path());
    }
    resImpl->parseResponse();
    // Write Response
    std::stringstream ss2;
    ss2 << "http::async_write response\n" << *resBeast << "\n";
    logStatus("::Connection::run", ss2.str());
    http::async_write(sslStream, *resBeast, yieldContext[ec]);
    if (ec) {
      logStatus("::Connection::run", "http::async_write", ec);
      return -1;
    }

    // We're done with the response so delete it
    resBeast.reset();
  }

  // Set the timeout.
  beast::get_lowest_layer(sslStream).expires_after(std::chrono::seconds(30));

  // Perform the SSL shutdown
  sslStream.async_shutdown(yieldContext[ec]);
  if (ec) {
    logStatus("::Connection::run", "sslStream.async_shutdown", ec);
    return -1;
  }

  // At this point the connection is closed gracefully
  return 0;
}

int Connection::badRequest(requestBeastInternal req, responseBeastInternal res,
                           beast::string_view what) {
  res->result(http::status::bad_request);
  res->version(req->version());
  res->set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res->set(http::field::content_type, "text/html");
  res->keep_alive(req->keep_alive());
  res->body() = std::string(what);
  res->content_length(res->body().length());
  res->prepare_payload();
  return 0;
}

int Connection::notFound(requestBeastInternal req, responseBeastInternal res,
                         beast::string_view what) {
  logStatus("::Connection::notFound", "START");
  std::this_thread::sleep_for(std::chrono::seconds(5));

  res->result(http::status::not_found);
  res->version(req->version());
  res->set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res->set(http::field::content_type, "text/html");
  res->keep_alive(req->keep_alive());
  res->body() = "The resource '" + std::string(what) + "' was not found.";
  res->content_length(res->body().length());
  res->prepare_payload();
  return 0;
}

int Connection::serverError(requestBeastInternal req, responseBeastInternal res,
                            beast::string_view what) {
  res->result(http::status::internal_server_error);
  res->version(req->version());
  res->set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res->set(http::field::content_type, "text/html");
  res->keep_alive(req->keep_alive());
  res->body() = "An error occurred: '" + std::string(what) + "'";
  res->content_length(res->body().length());
  res->prepare_payload();
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
