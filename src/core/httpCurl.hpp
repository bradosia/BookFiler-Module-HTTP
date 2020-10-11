/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_H
#define BOOKFILER_MODULE_HTTP_HTTP_H

// Local Project
#include "Curl.hpp"

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

class ConnectionImpl : public Connection {
private:
  std::string url;
  std::shared_ptr<rapidjson::Value> settingsDoc;
  std::shared_ptr<rapidjson::Document> responseJSON_Doc;

public:
  ConnectionImpl();
  ~ConnectionImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value>);
  int setURL(std::string);
  int exec();
};

/* Makes an HTTPS GET request to the URI
 * @param URI The address
 */
void printJSON_value(const rapidjson::Value &a, unsigned int depth);
void printJSON_iterator(rapidjson::Value::ConstMemberIterator &itr,
                        unsigned int depth);

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
