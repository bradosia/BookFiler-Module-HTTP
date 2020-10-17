/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_SESSION_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_SESSION_H

// config
#include "config.hpp"

// C++17
//#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

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
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/signals2.hpp>

// Local Project
#include "httpServerUtil.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl; // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

// Handles an HTTP server connection
class Session : public std::enable_shared_from_this<Session> {
private:
  std::shared_ptr<rapidjson::Document> sessionDocument;
  beast::ssl_stream<beast::tcp_stream> sslStream;
  beast::flat_buffer buffer;
  std::shared_ptr<std::string const> docRoot;
  std::shared_ptr<http::request<http::string_body>> req;
  std::shared_ptr<beast::http::response<beast::http::string_body>> res;
  std::shared_ptr<routeSignalType> routeSignal;

public:
  // Take ownership of the socket
  Session(tcp::socket &&socket, ssl::context &ctx,
          std::shared_ptr<std::string const> const &doc_root);

  // Start the asynchronous operation
  void run();
  void on_run();
  void on_handshake(beast::error_code ec);
  void do_read();
  void on_read(beast::error_code ec, std::size_t bytes_transferred);
  void on_write(bool close, beast::error_code ec,
                std::size_t bytes_transferred);
  void do_close();
  void on_shutdown(beast::error_code ec);
  int setRouteSignal(std::shared_ptr<routeSignalType>);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
