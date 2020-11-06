/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_COOKIE_MAP_H
#define BOOKFILER_MODULE_HTTP_HTTP_COOKIE_MAP_H

// config
#include "config.hpp"

// C++17
//#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <map>
#include <utility>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/algorithm/string.hpp>

// Local Project
#include "RequestCookie.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class CookieMapImpl {
private:
  std::map<std::string, CookieImpl> cookieMap;

public:
  CookieMapImpl();
  ~CookieMapImpl();
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_HTTP_COOKIE_H
