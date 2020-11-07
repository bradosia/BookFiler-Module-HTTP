/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_RESPONSE_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_RESPONSE_H

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
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

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

// Local Project
#include "RequestCookieMap.hpp"
#include "json.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

using responseBeastInternal = std::shared_ptr<
    boost::beast::http::response<boost::beast::http::string_body>>;

class ResponseImpl : public Response {
private:
  responseBeastInternal resBeast;
  bool writableEndedFlag;
  std::shared_ptr<std::string> bodyStrPtr;
  CookieMapImpl cookieMap;

public:
  ResponseImpl();
  ~ResponseImpl();
  responseBeastInternal getResponse();
  int setResponse(responseBeastInternal);

  /* This method signals to the server that all of the response headers and body
   * have been sent; that server should consider this message complete.
   */
  int end(std::string);
  /* Is true after response.end() has been called. This property does not
   * indicate whether the data has been flushed, for this use
   * response.writableFinished instead.
   */
  bool writableEnded();
  std::shared_ptr<std::string> body();
  /* Called before writing the boost::beast::http::message
   * Writes the data in this Reponse object back to the underlying
   * boost::beast::http::message
   */
  int parseResponse();
  void nothing();
  // cookie methods
  int setCookie(std::string, std::string,
                std::map<std::string, newCookieVariantType>);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
