/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_PORT_FORWARDING_UTIL_H
#define BOOKFILER_MODULE_PORT_FORWARDING_UTIL_H

// config
#include "config.hpp"

// C++17
#include <filesystem>s
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/algorithm/string.hpp>

// Local Project
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - port
 * Port fowarding utilities. Initially this utility was made to forward mysql
 * traffic through a secure ssh connection.
 */
namespace bookfiler {
namespace port {

extern std::mutex globalMutex;

void logStatus(std::string functionStr, std::string msg);
void logStatus(std::string functionStr, std::string msg, boost::system::error_code ec);
std::string readFile(std::string path);

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
