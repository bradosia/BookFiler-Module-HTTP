/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_HEADER_H
#define BOOKFILER_MODULE_HTTP_HTTP_HEADER_H

// config
#include "config.hpp"

// C++17
#include <iostream>
#include <string>
#include <unordered_map>

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

class HeaderImpl {
private:
  std::unordered_map<std::string, std::string> headerMap;
  std::string headerStr;

public:
  HeaderImpl();
  HeaderImpl(std::string);
  ~HeaderImpl();

  int setHeader(std::unordered_map<std::string, std::string>);
  std::string flattenHeaderMap();
  int generateHeaderMap(std::string);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_HTTP_HEADER_H
