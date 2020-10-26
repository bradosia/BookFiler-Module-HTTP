/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServerUtil.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path) {
  using beast::iequals;
  auto const ext = [&path] {
    auto const pos = path.rfind(".");
    if (pos == beast::string_view::npos)
      return beast::string_view{};
    return path.substr(pos);
  }();
  if (iequals(ext, ".htm"))
    return "text/html";
  if (iequals(ext, ".html"))
    return "text/html";
  if (iequals(ext, ".php"))
    return "text/html";
  if (iequals(ext, ".css"))
    return "text/css";
  if (iequals(ext, ".txt"))
    return "text/plain";
  if (iequals(ext, ".js"))
    return "application/javascript";
  if (iequals(ext, ".json"))
    return "application/json";
  if (iequals(ext, ".xml"))
    return "application/xml";
  if (iequals(ext, ".swf"))
    return "application/x-shockwave-flash";
  if (iequals(ext, ".flv"))
    return "video/x-flv";
  if (iequals(ext, ".png"))
    return "image/png";
  if (iequals(ext, ".jpe"))
    return "image/jpeg";
  if (iequals(ext, ".jpeg"))
    return "image/jpeg";
  if (iequals(ext, ".jpg"))
    return "image/jpeg";
  if (iequals(ext, ".gif"))
    return "image/gif";
  if (iequals(ext, ".bmp"))
    return "image/bmp";
  if (iequals(ext, ".ico"))
    return "image/vnd.microsoft.icon";
  if (iequals(ext, ".tiff"))
    return "image/tiff";
  if (iequals(ext, ".tif"))
    return "image/tiff";
  if (iequals(ext, ".svg"))
    return "image/svg+xml";
  if (iequals(ext, ".svgz"))
    return "image/svg+xml";
  return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path) {
  if (base.empty())
    return std::string(path);
  std::string result(base);
#ifdef BOOST_MSVC
  char constexpr path_separator = '\\';
  if (result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
  for (auto &c : result)
    if (c == '/')
      c = path_separator;
#else
  char constexpr path_separator = '/';
  if (result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
#endif
  return result;
}

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const *what) {
  // ssl::error::stream_truncated, also known as an SSL "short read",
  // indicates the peer closed the connection without performing the
  // required closing handshake (for example, Google does this to
  // improve performance). Generally this can be a security issue,
  // but if your communication protocol is self-terminated (as
  // it is with both HTTP and WebSocket) then you may simply
  // ignore the lack of close_notify.
  //
  // https://github.com/boostorg/beast/issues/38
  //
  // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
  //
  // When a short read would cut off the end of an HTTP message,
  // Beast returns the error beast::http::error::partial_message.
  // Therefore, if we see a short read here, it has occurred
  // after the message has been completed, so it is safe to ignore it.

  if (ec == net::ssl::error::stream_truncated)
    return;

  std::cerr << what << ": " << ec.message() << "\n";
}

int badRequest(requestBeastInternal req, responseBeastInternal res,
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

int notFound(requestBeastInternal req, responseBeastInternal res,
             beast::string_view what) {
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

int serverError(requestBeastInternal req, responseBeastInternal res,
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

int getRequest(requestBeastInternal req, responseBeastInternal res,
               beast::string_view why) {
  res->result(http::status::ok);
  res->version(req->version());
  res->set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res->set(http::field::content_type, "text/html");
  res->keep_alive(req->keep_alive());
  res->body() = std::string(why);
  res->content_length(res->body().length());
  res->prepare_payload();
  return 0;
}

int handleRequest(std::shared_ptr<Session> session, requestBeastInternal req,
                  responseBeastInternal res,
                  std::shared_ptr<routeSignalTypeInternal> routeSignal) {
  std::cout << "handleRequest" << std::endl;
  // Make sure we can handle the method
  if (req->method() != http::verb::get && req->method() != http::verb::head) {
    return badRequest(req, res, "Unknown HTTP-method");
  }
  // Request path must be absolute and not contain "..".
  if (req->target().empty() || req->target()[0] != '/' ||
      req->target().find("..") != beast::string_view::npos) {
    return badRequest(req, res, "Illegal request-target");
  }
  (*routeSignal)(session, req, res);
  // Respond to HEAD request
  /*if (req->method() == http::verb::head) {
    getRequest(req, res, "TESTING RESPONSE");
  }
  getRequest(req, res, "TESTING RESPONSE");*/
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
