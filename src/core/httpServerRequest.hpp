/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_REQUEST_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_REQUEST_H

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

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/asio/spawn.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/signals2.hpp>

// Local Project
#include "httpCookieMap.hpp"
#include "httpUrl.hpp"
#include "json.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

using requestBeastInternal = std::shared_ptr<
    boost::beast::http::request<boost::beast::http::string_body>>;

class RequestImpl : public Request {
private:
  requestBeastInternal requestBeast;
  CookieMapImpl cookieMap;

public:
  RequestImpl();
  ~RequestImpl();
  std::shared_ptr<UrlImpl> urlPtr;
  std::string methodStr;
  std::string_view refererStr, userAgentStr;
  // url methods
  std::string_view url();
  std::string_view method();
  std::string_view host();
  std::string path();
  // header methods
  int setHeader(std::unordered_map<std::string, std::string>);
  // query methods
  std::string getEncodedQuery();
  std::optional<std::string> getQuery(std::string);
  // header methods

  // boost beast methods
  requestBeastInternal getRequest();
  int setRequest(requestBeastInternal);
  /* Use after setRequest to populate the convenience features
   * not included with boost::beast::http::request
   */
  int parseRequest();
  /* Arguments provided are from the routing function arguments
   * They will be used to match requests.
   */
  int routeValidate(std::string_view method, std::string_view path);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
