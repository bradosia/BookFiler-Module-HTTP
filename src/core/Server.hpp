/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_H

// config
#include "config.hpp"

// C++17
//#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <map>
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
#include "certificateManager.hpp"
#include "ServerListener.hpp"

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

class ServerImpl : public Server {
private:
  std::shared_ptr<rapidjson::Value> settingsDoc;
  std::shared_ptr<ServerState> serverState;
  std::shared_ptr<boost::asio::io_context> ioContext;
  std::shared_ptr<boost::asio::ssl::context> sslContext;
  std::shared_ptr<Listener> listener;
  std::vector<std::thread> threadList;
  std::shared_ptr<RouteImpl> routePtr;

public:
  ServerImpl();
  ~ServerImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value>);
  int run();
  void runIoContext();
  int extractSettings();
  int useCertificate(std::shared_ptr<bookfiler::certificate::Certificate>);
  int route(std::map<std::string, routeVariantTypeExternal> map_);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
