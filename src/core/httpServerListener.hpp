/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_LISTENER_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_LISTENER_H

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
#include "httpServerSession.hpp"
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

// Accepts incoming connections and launches the sessions
class Listener : public std::enable_shared_from_this<Listener> {
  net::io_context &ioContext;
  ssl::context &sslContext;
  tcp::acceptor acceptor;
  std::shared_ptr<std::string const> docRoot;
  std::shared_ptr<routeSignalType> routeSignal;
  std::shared_ptr<Session> session;

public:
  Listener(net::io_context &ioContext_, ssl::context &sslContext_,
           tcp::endpoint endpoint,
           std::shared_ptr<std::string const> const &docRoot);

  // Start accepting incoming connections
  void run();
  int setRouteSignal(std::shared_ptr<routeSignalType>);

private:
  void do_accept();
  void on_accept(beast::error_code ec, tcp::socket socket);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
