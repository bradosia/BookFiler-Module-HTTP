/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_URL_H
#define BOOKFILER_MODULE_HTTP_HTTP_URL_H

// config
#include "config.hpp"

// C++17
//#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

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
#include <boost/algorithm/string.hpp>
#define BOOST_URL_HEADER_ONLY 1
#include <boost/url.hpp>

// Local Project
#include "httpQuery.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class UrlImpl : public Url, public QueryImpl, public boost::url {
private:
  int setEncodedQueryBase(std::string);

public:
  UrlImpl();
  UrlImpl(std::unordered_map<std::string, newUrlVariantType>);
  ~UrlImpl();
  // Query Methods
  int setQuery(std::unordered_map<std::string, std::string>);
  int setQuery(std::shared_ptr<rapidjson::Document>);
  int setEncodedQuery(std::string);
  std::string getEncodedQuery();
  std::optional<std::string> getQuery(std::string);
  // Url Methods
  std::string_view url();
  std::string_view getEncodedHost();
  std::string target();
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
