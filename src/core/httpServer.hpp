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
#include "certificateManager.hpp"
#include "httpServerListener.hpp"

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
  boost::asio::ip::address address;
  std::shared_ptr<std::string> docRoot;
  std::shared_ptr<boost::asio::io_context> ioContext;
  std::shared_ptr<boost::asio::ssl::context> sslContext;
  std::shared_ptr<Listener> listener;
  unsigned short port;
  int threadsNum;

public:
  boost::signals2::signal<int(std::string)> requestSignal;
  std::shared_ptr<routeSignalTypeInternal> routeSignal;

  ServerImpl();
  ~ServerImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value>);
  int run();
  int runAsync();
  int extractSettings();
  void runIoContext();
  int useCertificate(std::shared_ptr<bookfiler::certificate::Certificate>);
  int route(std::unordered_map<std::string, routeVariantType_Beast> map);
  int route(std::unordered_map<std::string, routeVariantType_NoBeast> map);
  int routeSignalAdd(
      std::shared_ptr<routeFunctionExpressType> routeFunctionExpress,
      std::string method, std::string path);
  std::shared_ptr<routeSignalTypeInternal> getRouteSignal();
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
