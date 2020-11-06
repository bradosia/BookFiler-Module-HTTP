/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_REQUEST_USER_AGENT_H
#define BOOKFILER_MODULE_HTTP_REQUEST_USER_AGENT_H

// config
#include "config.hpp"

// C++17
//#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>

// Local Project
#include "UaParser.h"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class UserAgentImpl {
private:
  uap_cpp::UserAgent userAgent;

public:
  UserAgentImpl();
  ~UserAgentImpl();
  std::string getBrowser();
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_REQUEST_USER_AGENT_H
