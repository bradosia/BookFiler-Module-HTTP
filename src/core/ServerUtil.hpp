/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_UTIL_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_UTIL_H

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
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
// #include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
// #include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/signals2.hpp>

// Local Project
#include "Request.hpp"
#include "Response.hpp"

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

class RouteCombiner {
private:
  std::shared_ptr<RequestImpl> req;
  std::shared_ptr<ResponseImpl> res;

public:
  typedef std::string result_type;

  template <typename InputIterator>
  std::string operator()(InputIterator first, InputIterator last) const {
    // If there are no slots to call, just return the
    // default-constructed value
    if (first == last) {
      return {};
    }
    std::string valueLast{};
    while (first != last) {
      valueLast = *first;
      if (res->writableEnded()) {
        logStatus("::RouteCombiner::operator()", "writableEnded()");
        return *(res->body());
      }
      ++first;
    }

    return valueLast;
  }

  int setRequest(std::shared_ptr<RequestImpl> req_) {
    req = req_;
    return 0;
  }
  int setResponse(std::shared_ptr<ResponseImpl> res_) {
    res = res_;
    return 0;
  }
};

using routeFunctionTypeExternal = std::function<std::string(
    std::shared_ptr<Request>, std::shared_ptr<Response>)>;
using routeSignalTypeInternal =
    boost::signals2::signal<std::string(std::shared_ptr<Request>,
                                        std::shared_ptr<Response>),
                            RouteCombiner>;
using routeFunctionTypeInternal = std::function<std::string(
    std::shared_ptr<Request>, std::shared_ptr<Response>)>;

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path);

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
