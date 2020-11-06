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

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>

// Local Project
#include "httpServerRequest.hpp"
#include "httpServerResponse.hpp"
#include "httpUrl.hpp"

namespace beast = boost::beast;
namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

class ClientImpl : public Client {
private:
  std::shared_ptr<UrlImpl> urlPtr;
  std::shared_ptr<rapidjson::Value> settingsDoc;
  std::shared_ptr<rapidjson::Document> responseJsonDoc;
  std::string method, CaInfoPath;
  std::string_view responseStr;
  std::shared_ptr<std::unordered_map<std::string, std::string>> headersMapPtr;
  std::shared_ptr<RequestImpl> requestPtr;
  std::shared_ptr<ResponseImpl> responsePtr;
  std::shared_ptr<boost::beast::http::request<boost::beast::http::string_body>> requestBeast;
  std::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>>
      responseBeast;
  bool skipPeerVerification, skipHostnameVerification;

  // boost beast
  std::unique_ptr<ssl::stream<tcp::socket>>
  connect(asio::io_context &ctx, ssl::context &ssl_ctx,
          std::string const &hostname);

public:
  ClientImpl();
  ClientImpl(std::unordered_map<std::string, newClientVariantType> map);
  ~ClientImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value>);
  int loadSettingsDoc();
  std::string_view url();
  std::string_view getEncodedHost();
  int setURL(std::string);
  int setQuery(std::unordered_map<std::string, std::string>);
  int setHeader(std::unordered_map<std::string, std::string>);
  int setCookie(std::unordered_map<std::string, std::string>);
  int setMethod(std::string method);
  std::optional<std::string_view> getResponseStr();
  std::optional<std::shared_ptr<rapidjson::Document>> getResponseJson();
  int end();
  int endAsync();
  int wait();
};

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
