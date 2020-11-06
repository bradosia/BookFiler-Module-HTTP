/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_CONNECTION_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_CONNECTION_H

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
#include <boost/config.hpp>
#include <boost/signals2.hpp>

// Local Project
#include "httpServerRoute.hpp"
#include "httpServerSession.hpp"
#include "httpServerState.hpp"
#include "httpServerUtil.hpp"

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

class ServerState;

// Handles an HTTP server connection
class Connection {
private:
  beast::ssl_stream<beast::tcp_stream> sslStream;
  std::shared_ptr<ServerState> serverState;
  std::shared_ptr<RouteImpl> routePtr;

public:
  // Take ownership of the socket
  Connection(tcp::socket, ssl::context &, std::shared_ptr<ServerState>,
             std::shared_ptr<RouteImpl>);
  ~Connection();

  // Start the asynchronous operation
  int run(net::yield_context yieldContext);

  int badRequest(requestBeastInternal req, responseBeastInternal res,
             beast::string_view what);
  int notFound(requestBeastInternal req, responseBeastInternal res,
           beast::string_view what);
  int serverError(requestBeastInternal req, responseBeastInternal res,
              beast::string_view what);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
