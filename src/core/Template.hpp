/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_TEMPLATE_H
#define BOOKFILER_MODULE_HTTP_TEMPLATE_H

// config
#include "config.hpp"

// C++17
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/config.hpp>
#include <boost/unordered_map.hpp>

/* Mustache 4.1
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <Mustache/mustache.hpp>

// Local Project
#include "json.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - Template
 */
namespace bookfiler {

class TemplateImpl : public Template {
private:
  std::shared_ptr<rapidjson::Value> settingsDoc;
  kainjow::mustache::data data;
  std::string templateStr, parsedStr;

public:
  TemplateImpl();
  ~TemplateImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value>);
  int readFile(std::string);
  int mergeData(object);
  int mergeData(rapidjson::Document &);
  int addContext(std::string, std::string);
  int parse();
  std::string toString();
};

} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_TEMPLATE_H
