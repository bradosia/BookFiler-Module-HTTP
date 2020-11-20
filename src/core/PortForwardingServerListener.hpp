/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_PORT_FORWARDING_SERVER_LISTENER_H
#define BOOKFILER_MODULE_PORT_FORWARDING_SERVER_LISTENER_H

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
#include "PortForwardingServerConnection.hpp"

/*
 * bookfiler - port
 * Port fowarding utilities. Initially this utility was made to forward mysql
 * traffic through a secure ssh connection.
 */
namespace bookfiler {
namespace port {

// Accepts incoming connections and launches the sessions
class Listener {
private:
  net::io_context &ioContext;
  ssl::context &sslContext;
  tcp::endpoint endpoint;
  std::shared_ptr<ServerState> serverState;
  std::shared_ptr<RouteImpl> routePtr;

public:
  Listener(net::io_context &, ssl::context &, tcp::endpoint,
           std::shared_ptr<ServerState>, std::shared_ptr<RouteImpl>);

  // Start accepting incoming connections
  int run(net::yield_context yieldContext);
};

} // namespace port
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_PORT_FORWARDING_SERVER_LISTENER_H
