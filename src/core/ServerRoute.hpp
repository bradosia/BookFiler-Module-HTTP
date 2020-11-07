/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_ROUTE_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_ROUTE_H

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
#include <boost/config.hpp>
#include <boost/signals2.hpp>

// Local Project
#include "Request.hpp"
#include "Response.hpp"
#include "ServerUtil.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class RouteImpl {
private:
  /* Routes are stored in a map so that on average there is a log n search
   * use "*" as the method to catch any method, and "*" as the path to catch any
   * path
   * "*" is the last priority.
   */
  /*std::map<std::string, std::shared_ptr<routeSignalTypeInternal>> routePostMap,
      routeGetMap, routeAllMap;*/

public:
  RouteImpl();
  ~RouteImpl();
  int getAdd(std::string path, int priority,
             routeFunctionTypeInternal routeFunction);
  int postAdd(std::string path, int priority,
              routeFunctionTypeInternal routeFunction);
  int allAdd(std::string path, int priority,
             routeFunctionTypeInternal routeFunction);
 /* int add(std::string path, int priority,
          routeFunctionTypeInternal routeFunction,
          std::map<std::string, std::shared_ptr<routeSignalTypeInternal>> &);*/
  int doGetSignal(std::string path, std::shared_ptr<RequestImpl> req,
                  std::shared_ptr<ResponseImpl> res);
  /*int doSignal(
      std::string path, std::shared_ptr<RequestImpl> req,
      std::shared_ptr<ResponseImpl> res,
      std::map<std::string, std::shared_ptr<routeSignalTypeInternal>> &);*/
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
