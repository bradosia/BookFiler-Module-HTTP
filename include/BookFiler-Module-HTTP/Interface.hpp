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
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#endif
#include <boost/signals2.hpp>
#define BOOST_URL_HEADER_ONLY 1
#include <boost/url.hpp>
#include <boost/url/url.hpp>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

/*
 * bookfiler - Curl
 */
namespace bookfiler {
namespace curl {

class Init {
public:
  Init(){};
  ~Init(){};
};

} // namespace curl
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

class Url {
public:
  virtual int setBase(std::string) = 0;
  virtual int setFields(
      std::shared_ptr<std::unordered_map<std::string, std::string>>) = 0;
  virtual int setFields(std::shared_ptr<rapidjson::Document>) = 0;
  virtual std::string getURL() = 0;
  virtual std::string getFieldsStr() = 0;
};

using newClientVariantType = std::variant<int, double, std::string>;

class Client {
public:
  boost::signals2::signal<int(std::shared_ptr<rapidjson::Document>)>
      jsonReceivedSignal;
  boost::signals2::signal<int(std::string)> dataReceivedSignal;
  virtual int setURL(std::string) = 0;
  virtual int setFields(
      std::shared_ptr<std::unordered_map<std::string, std::string>>) = 0;
  virtual int setHeaders(
      std::shared_ptr<std::unordered_map<std::string, std::string>>) = 0;
  virtual int setMethod(std::string) = 0;
  virtual int exec() = 0;
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
  virtual std::string query() = 0;
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
  virtual requestBeast getRequest() = 0;
#endif
};

class Response {
public:
  /* Sends the HTTP response.
   */
  virtual int send(std::string) = 0;
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
  virtual responseBeast getResponse() = 0;
#endif
};

class Session {
public:
  virtual int routeValidate(std::shared_ptr<Request> req,
                            std::string_view method, std::string_view path) = 0;
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
  virtual std::shared_ptr<Request> parseRequest(requestBeast reqBeast) = 0;
#endif
};

using request = std::shared_ptr<Request>;
using response = std::shared_ptr<Response>;

#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
using routeSignalType = boost::signals2::signal<int(
    std::shared_ptr<Session>, requestBeast, responseBeast)>;
using routeFunctionBeastType =
    std::function<int(std::shared_ptr<Session>, requestBeast, responseBeast)>;
#endif
using routeFunctionExpressType = std::function<std::string(request, response)>;

// makes the arguments look like JSON
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
using routeVariantType =
    std::variant<int, double, std::string, routeFunctionExpressType,
                 routeFunctionBeastType>;
#else
using routeVariantType =
    std::variant<int, double, std::string, routeFunctionExpressType>;
#endif
using routeObjectType = std::unordered_map<std::string, routeVariantType>;
using routeArrayType = std::vector<routeVariantType>;

using newServerVariantType = std::variant<int, double, std::string>;

class Server {
public:
  // virtual int run() = 0;
  virtual int runAsync() = 0;
  virtual int
      useCertificate(std::shared_ptr<bookfiler::certificate::Certificate>) = 0;
  virtual int route(std::unordered_map<std::string, routeVariantType> map) = 0;
#if BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE
  virtual std::shared_ptr<routeSignalType> getRouteSignal() = 0;
#endif
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
      newClient(std::unordered_map<std::string, newClientVariantType>) = 0;
  virtual std::shared_ptr<Url> newUrl() = 0;
  virtual std::shared_ptr<Server>
      newServer(std::unordered_map<std::string, newServerVariantType>) = 0;
  virtual std::shared_ptr<bookfiler::certificate::Manager>
  newCertificateManager() = 0;
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_INTERFACE_H
