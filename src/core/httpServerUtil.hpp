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
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/signals2.hpp>

// Local Project
#include "httpServerRequest.hpp"
#include "httpServerResponse.hpp"

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

using routeSignalTypeInternal = boost::signals2::signal<int(
    std::shared_ptr<Session>, requestBeastInternal, responseBeastInternal)>;
using routeFunctionBeastTypeInternal = std::function<int(
    std::shared_ptr<Session>, requestBeastInternal, responseBeastInternal)>;

using routeVariantType_Beast =
    std::variant<int, double, std::string, routeFunctionExpressType,
                 routeFunctionBeastTypeInternal>;
using routeVariantType_NoBeast =
    std::variant<int, double, std::string, routeFunctionExpressType>;

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path);

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path);

// Report a failure
void fail(beast::error_code ec, char const *what);

int badRequest(requestBeastInternal req, responseBeastInternal res,
               beast::string_view why);

int notFound(requestBeastInternal req, responseBeastInternal res,
             beast::string_view why);

int serverError(requestBeastInternal req, responseBeastInternal res,
                beast::string_view why);

int getRequest(requestBeastInternal req, responseBeastInternal res,
               beast::string_view why);

int handleRequest(std::shared_ptr<Session>, requestBeastInternal req,
                  responseBeastInternal res,
                  std::shared_ptr<routeSignalTypeInternal> routeSignal);

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
