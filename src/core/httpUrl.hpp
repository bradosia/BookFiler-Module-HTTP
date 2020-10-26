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
//#include <filesystem>
#include <fstream>
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

// Local Project
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

class UrlImpl : public Url, public boost::url {
private:
  std::string base;
  std::shared_ptr<std::unordered_map<std::string, std::string>> fieldsMapPtr;

public:
  UrlImpl();
  ~UrlImpl();
  int setBase(std::string);
  int setFields(std::shared_ptr<std::unordered_map<std::string, std::string>>);
  int setFields(std::shared_ptr<rapidjson::Document>);
  std::string getURL();
  std::string getBase();
  std::string getFieldsStr();
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
