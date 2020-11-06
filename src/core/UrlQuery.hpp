/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_QUERY_H
#define BOOKFILER_MODULE_HTTP_HTTP_QUERY_H

// config
#include "config.hpp"

// C++17
#include <iostream>
#include <string>
#include <map>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/algorithm/string.hpp>
#define BOOST_URL_HEADER_ONLY 1
#include <boost/url.hpp>

// Local Project
#include "Util.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class QueryImpl {
private:
  std::map<std::string, std::string> queryMap;
  std::string queryEncodedStr;
  Util util;

public:
  QueryImpl();
  QueryImpl(std::string);
  ~QueryImpl();

  int setQuery(std::map<std::string, std::string>);
  int setQuery(std::shared_ptr<rapidjson::Document>);
  int setEncodedQuery(std::string);
  std::string getQuery();
  std::string getEncodedQuery();
  std::optional<std::string> getQuery(std::string);
  std::string flattenQueryMap();
  int generateQueryMap(std::string);
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_HTTP_QUERY_H
