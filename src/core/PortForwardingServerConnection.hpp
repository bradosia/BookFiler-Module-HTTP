/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */
#ifndef BOOKFILER_MODULE_PORT_FORWARDING_SERVER_CONNECTION_H
#define BOOKFILER_MODULE_PORT_FORWARDING_SERVER_CONNECTION_H

// config
#include "config.hpp"

// C++17
//#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
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

// Local Project
#include "PortForwardingServerState.hpp"
#include "PortForwardingUtil.hpp"

namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

/*
 * bookfiler - port
 * Port fowarding utilities. Initially this utility was made to forward mysql
 * traffic through a secure ssh connection.
 */
namespace bookfiler {
namespace port {

class ServerState;

// Handles an HTTP server connection
class Connection {
private:
  beast::ssl_stream<beast::tcp_stream> sslStream;
  std::shared_ptr<ServerState> serverState;

public:
  // Take ownership of the socket
  Connection(tcp::socket, std::shared_ptr<ServerState>);
  ~Connection();

  // Start the asynchronous operation
  int run(boost::asio::yield_context yieldContext);

  int badRequest(requestBeastInternal req, responseBeastInternal res,
                 beast::string_view what);
  int notFound(requestBeastInternal req, responseBeastInternal res,
               beast::string_view what);
  int serverError(requestBeastInternal req, responseBeastInternal res,
                  beast::string_view what);
};

} // namespace port
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
