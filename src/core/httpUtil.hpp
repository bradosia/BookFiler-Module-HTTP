/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_UTIL_H
#define BOOKFILER_MODULE_HTTP_HTTP_UTIL_H

// config
#include "config.hpp"

// C++17
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/algorithm/string.hpp>

// Local Project
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class Util {
private:
  const std::string illegalCharStr = "%<>{}|\\\"^`";

public:
  Util();
  ~Util();
  int uriEncode(const std::string &str, const std::string &reserved,
                std::string &encodedStr);
  int uriDecode(const std::string &str, std::string &decodedStr);
};

//Util util;

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
