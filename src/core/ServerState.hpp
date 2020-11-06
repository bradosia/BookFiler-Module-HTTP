/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_STATE_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_STATE_H

// config
#include "config.hpp"

// C++17
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <map>
#include <utility>
#include <vector>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/asio/ip/address.hpp>

// Local Project
#include "ServerConnection.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class Connection;

class ServerState {
public:
  ServerState();
  ~ServerState();
  unsigned short port;
  int threadsNum, portInt;
  boost::asio::ip::address address;
  std::string docRootStr, addressStr;
  std::vector<std::shared_ptr<Connection>> connectionList;
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_HTTP_SERVER_STATE_H
