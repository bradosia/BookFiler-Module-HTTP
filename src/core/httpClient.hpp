/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_CLIENT_H
#define BOOKFILER_MODULE_HTTP_HTTP_CLIENT_H

// config
#include "config.hpp"

// Local Project
#include "Curl.hpp"
#include "httpUrl.hpp"

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

class ClientImpl : public Client {
private:
  std::shared_ptr<UrlImpl> urlPtr;
  std::shared_ptr<rapidjson::Value> settingsDoc;
  std::shared_ptr<rapidjson::Document> responseJSON_Doc;
  std::string method;
  std::shared_ptr<std::unordered_map<std::string, std::string>> headersMapPtr;

public:
  ClientImpl();
  ClientImpl(std::unordered_map<std::string, newClientVariantType> map);
  ~ClientImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value>);
  int setURL(std::string);
  int setFields(std::shared_ptr<std::unordered_map<std::string, std::string>>);
  int setHeaders(std::shared_ptr<std::unordered_map<std::string, std::string>>);
  int setMethod(std::string method);
  int exec();
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
