/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_SESSION_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_SESSION_H

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

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

/* This session is passed when routing with raw boost::beast::http objects.
 * This class provides convenience functions to help with routing and request parsing
 */
class SessionImpl : public Session {
private:
public:
  int routeValidate(std::shared_ptr<Request> req, std::string_view method,
                    std::string_view path);
  std::shared_ptr<Request> parseRequest(requestBeast reqBeast);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
