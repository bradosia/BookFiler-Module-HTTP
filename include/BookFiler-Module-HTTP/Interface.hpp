/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_INTERFACE_H
#define BOOKFILER_MODULE_HTTP_INTERFACE_H

// c++17
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#endif
//#include <boost/signals2.hpp>
//#define BOOST_URL_HEADER_ONLY 1
//#include <boost/url.hpp>
//#include <boost/url/url.hpp>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

/*
 * bookfiler - Json, Template
 */
namespace bookfiler {
class Json {
public:
  virtual std::optional<rapidjson::Document> readFile(std::string) = 0;
};

using templateVariantType = std::variant<int, double, std::string>;
using array = std::vector<templateVariantType>;
using object = std::unordered_map<std::string, templateVariantType>;

class Template {
public:
  virtual int readFile(std::string) = 0;
  virtual int mergeData(object) = 0;
  virtual int mergeData(rapidjson::Document &) = 0;
  virtual int parse() = 0;
  virtual int addContext(std::string, std::string) = 0;
  virtual std::string toString() = 0;
};

} // namespace bookfiler

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

class Certificate {
public:
  virtual std::string getInfo() = 0;
};

class Manager {
public:
  virtual int createX509Store() = 0;
  virtual int newCertificate(std::shared_ptr<Certificate> &,
                             std::shared_ptr<rapidjson::Document>) = 0;
  virtual int newRequest(std::shared_ptr<Certificate> &,
                         std::shared_ptr<rapidjson::Document>) = 0;
  virtual int signRequest(std::shared_ptr<Certificate>,
                          std::shared_ptr<Certificate>,
                          std::shared_ptr<rapidjson::Document>) = 0;
  virtual int newCertRootLocalhost(std::shared_ptr<Certificate> &,
                                   std::shared_ptr<rapidjson::Document>) = 0;
  virtual int newCertServerLocalhost(std::shared_ptr<Certificate> &,
                                     std::shared_ptr<rapidjson::Document>) = 0;
  virtual int addCertificate(std::shared_ptr<Certificate>) = 0;
  virtual int saveCertificate(std::shared_ptr<Certificate>, std::string) = 0;
  virtual int loadCertificate(std::shared_ptr<Certificate> &) = 0;
};

} // namespace certificate
} // namespace bookfiler

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

class Cookie {
public:
  virtual std::string_view value() = 0;
};

using newUrlVariantType = std::variant<int, double, std::string>;

class Url {
public:
  virtual int setQuery(std::map<std::string, std::string>) = 0;
  virtual int setQuery(std::shared_ptr<rapidjson::Document>) = 0;
  virtual std::string getEncodedQuery() = 0;
  virtual std::optional<std::string> getQuery(std::string) = 0;
  virtual std::string_view url() = 0;
};

using newClientVariantType = std::variant<int, double, std::string>;

class Client {
public:
  // url methods
  virtual std::string_view url() = 0;
  virtual int setURL(std::string) = 0;
  virtual int setQuery(std::map<std::string, std::string>) = 0;
  virtual int setHeader(std::map<std::string, std::string>) = 0;
  virtual int setMethod(std::string) = 0;
  // client methods
  virtual std::optional<std::string_view> getResponseStr() = 0;
  virtual std::optional<std::shared_ptr<rapidjson::Document>>
  getResponseJson() = 0;
  virtual int end() = 0;
  virtual int endAsync() = 0;
  virtual int wait() = 0;
};

#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
using requestBeast = std::shared_ptr<
    boost::beast::http::request<boost::beast::http::string_body>>;
using responseBeast = std::shared_ptr<
    boost::beast::http::response<boost::beast::http::string_body>>;
#endif

class Request {
public:
  virtual std::string_view url() = 0;
  virtual std::string_view method() = 0;
  virtual std::string_view host() = 0;
  virtual std::string path() = 0;
  // header methods
  virtual int setHeader(std::map<std::string, std::string>) = 0;
  virtual std::string getEncodedQuery() = 0;
  virtual std::optional<std::string> getQuery(std::string) = 0;
  virtual int parseRequest() = 0;
  // cookie methods
  virtual std::shared_ptr<Cookie> getCookie(const std::string) = 0;
  // user agent methods
  virtual std::string getUserAgentBrowser() = 0;
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
  virtual requestBeast getRequest() = 0;
#endif
};

using newCookieVariantType = std::variant<int, double, std::string>;

class Response {
public:
  /* Sends the HTTP response.
   */
  virtual int end(std::string) = 0;
  virtual std::shared_ptr<std::string> body() = 0;
  virtual void nothing() = 0;
  virtual int setCookie(std::string, std::string,
                        std::map<std::string, newCookieVariantType>) = 0;
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
  virtual responseBeast getResponse() = 0;
#endif
};

class Session {
public:
};

using request = std::shared_ptr<Request>;
using response = std::shared_ptr<Response>;

// makes the arguments look like JSON
using routeFunctionTypeExternal = std::function<std::string(
    std::shared_ptr<Request>, std::shared_ptr<Response>)>;
using routeVariantTypeExternal =
    std::variant<int, double, std::string, routeFunctionTypeExternal>;

using routeObjectType = std::map<std::string, routeFunctionTypeExternal>;
using routeArrayType = std::vector<routeFunctionTypeExternal>;

using newServerVariantType = std::variant<int, double, std::string>;

class Server {
public:
  virtual int run() = 0;
  virtual int
      useCertificate(std::shared_ptr<bookfiler::certificate::Certificate>) = 0;
  virtual int route(std::map<std::string, routeVariantTypeExternal> map) = 0;
};

class ModuleInterface {
public:
  /* module typical methods
   * init, registerSettings
   */
  virtual int init() = 0;
  /* registerSettings
   * @brief Registers a map of callbacks for handling data from the main
   * application settings file.
   */
  virtual int registerSettings(
      std::shared_ptr<rapidjson::Document>,
      std::shared_ptr<std::unordered_map<
          std::string,
          std::function<void(std::shared_ptr<rapidjson::Document>)>>>) = 0;
  virtual std::shared_ptr<Client> newClient() = 0;
  virtual std::shared_ptr<Client>
      newClient(std::map<std::string, newClientVariantType>) = 0;
  virtual std::shared_ptr<Url>
      newUrl(std::map<std::string, newUrlVariantType>) = 0;
  virtual std::shared_ptr<Server>
      newServer(std::map<std::string, newServerVariantType>) = 0;
  virtual std::shared_ptr<bookfiler::certificate::Manager>
  newCertificateManager() = 0;
  virtual std::shared_ptr<bookfiler::Template> newTemplate() = 0;
  /* Json class has a bunch of "static" helper utilities for JSON
   * We could export the object and make it accessible to the user, but
   * It's too annoying for the module user to load in yet another module.
   * Instead we just return the object from the primary module.
   */
  virtual std::shared_ptr<bookfiler::Json> Json() = 0;
  virtual void wait(const std::string handle_) = 0;
  virtual void notify(const std::string handle_) = 0;
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_INTERFACE_H
