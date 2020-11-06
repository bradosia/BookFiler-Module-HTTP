/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "Client.hpp"

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

ClientImpl::ClientImpl() {
  urlPtr = std::make_shared<UrlImpl>();
  method = "GET";
  skipPeerVerification = skipHostnameVerification = false;
  // request
  requestBeast = std::make_shared<
      boost::beast::http::request<boost::beast::http::string_body>>();
  requestPtr = std::make_shared<RequestImpl>();
  requestPtr->setRequest(requestBeast);
};
ClientImpl::ClientImpl(
    std::map<std::string, newClientVariantType> map) {
  urlPtr = std::make_shared<UrlImpl>();
  for (auto val : map) {
    if (int *val_ = std::get_if<int>(&val.second)) {
    } else if (double *val_ = std::get_if<double>(&val.second)) {
    } else if (std::string *val_ = std::get_if<std::string>(&val.second)) {
      if (val.first == "method") {
        method = *val_;
      } else if (val.first == "host") {
        urlPtr->set_encoded_host(*val_);
      } else if (val.first == "path") {
        urlPtr->set_encoded_path(*val_);
      } else if (val.first == "scheme") {
        urlPtr->set_scheme(*val_);
      } else if (val.first == "query") {
        urlPtr->setEncodedQuery(*val_);
      }
    }
  }
  if (urlPtr->scheme().empty()) {
    if (method == "GET") {
      urlPtr->set_scheme("https");
    }
  }
#if BOOKFILER_HTTP_CLIENT_CLIENT_DEBUG_URL
  std::cout << "\n=== THREAD " << std::this_thread::get_id() << " ===\n"
            << moduleCode << "::ClientImpl::ClientImpl urlPtr->encoded_url: "
            << urlPtr->encoded_url() << std::endl;
#endif
  // request
  requestBeast = std::make_shared<
      boost::beast::http::request<boost::beast::http::string_body>>();
  requestPtr = std::make_shared<RequestImpl>();
  requestPtr->setRequest(requestBeast);
};
ClientImpl::~ClientImpl(){};

int ClientImpl::setSettingsDoc(std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}

int ClientImpl::loadSettingsDoc() {
  /* Get settings from JSON document */
  if (!settingsDoc->IsObject()) {
    std::thread::id threadId = std::this_thread::get_id();
    std::cout
        << "\n=== THREAD " << threadId << " ===\n"
        << moduleCode
        << "::ClientImpl::loadSettingsDoc ERROR:\nSettings document invalid"
        << std::endl;
    return -1;
  }
  char memberName01[] = "CaInfoPath";
  if (settingsDoc->HasMember(memberName01) &&
      (*settingsDoc)[memberName01].IsString()) {
    CaInfoPath = (*settingsDoc)[memberName01].GetString();
  }
  char memberName02[] = "skipPeerVerification";
  if (settingsDoc->HasMember(memberName02) &&
      (*settingsDoc)[memberName02].IsBool()) {
    skipPeerVerification = (*settingsDoc)[memberName02].GetBool();
  }
  char memberName03[] = "skipHostnameVerification";
  if (settingsDoc->HasMember(memberName03) &&
      (*settingsDoc)[memberName03].IsBool()) {
    skipHostnameVerification = (*settingsDoc)[memberName03].GetBool();
  }
  return 0;
}

std::string_view ClientImpl::url() { return urlPtr->url(); }

std::string_view ClientImpl::getEncodedHost() {
  return urlPtr->getEncodedHost();
}

int ClientImpl::setURL(std::string url_) {
  urlPtr->set_encoded_url(url_);
  return 0;
}
int ClientImpl::setQuery(
    std::map<std::string, std::string> fieldsMap_) {
  urlPtr->setQuery(fieldsMap_);
  return 0;
}

int ClientImpl::setHeader(
    std::map<std::string, std::string> headerMap_) {
  requestPtr->setHeader(headerMap_);
  return 0;
}

int ClientImpl::setCookie(
    std::map<std::string, std::string> cookieMap_) {
  return 0;
}

int ClientImpl::setMethod(std::string method_) {
  method = method_;
  return 0;
}

std::unique_ptr<ssl::stream<tcp::socket>>
ClientImpl::connect(asio::io_context &ctx, ssl::context &ssl_ctx,
                    std::string const &hostname) {
  // resolve
  tcp::resolver resolver{ctx};
  tcp::resolver::results_type resolved = resolver.resolve(hostname, "https");

  // socket
  tcp::socket socket{ctx};
  asio::connect(socket, resolved);

  auto streamPtr =
      boost::make_unique<ssl::stream<tcp::socket>>(std::move(socket), ssl_ctx);
  // tag::stream_setup_source[]
  boost::certify::set_server_hostname(*streamPtr, hostname);
  boost::certify::sni_hostname(*streamPtr, hostname);
  // end::stream_setup_source[]

  streamPtr->handshake(ssl::stream_base::handshake_type::client);
  return streamPtr;
}

std::optional<std::string_view> ClientImpl::getResponseStr() {
  if (!responseStr.empty()) {
    return responseStr;
  }
  return {};
}

std::optional<std::shared_ptr<rapidjson::Document>>
ClientImpl::getResponseJson() {
  if (responseJsonDoc) {
    return responseJsonDoc;
  }
  return {};
}

int ClientImpl::end() {
  const std::string hostname(urlPtr->getEncodedHost());

  std::thread::id threadId = std::this_thread::get_id();
  std::cout << "\n=== THREAD " << threadId << " ===\n"
            << moduleCode << "::ClientImpl::end Connection Settings:"
            << "\nhostname: " << hostname << "\nURL: " << urlPtr->url()
            << "\nURL Field String: " << urlPtr->getEncodedQuery()
            << "\nURL target: " << urlPtr->target()
            << "\nHTTP Method: " << method << "\nCaInfoPath: " << CaInfoPath
            << "\nskipPeerVerification: " << skipPeerVerification
            << "\nskipHostnameVerification: " << skipHostnameVerification
            << std::endl;

  asio::io_context ctx;
  ssl::context ssl_ctx{ssl::context::tls_client};
  ssl_ctx.set_verify_mode(ssl::context::verify_peer |
                          ssl::context::verify_fail_if_no_peer_cert);
  ssl_ctx.set_default_verify_paths();
  // tag::ctx_setup_source[]
  boost::certify::enable_native_https_server_verification(ssl_ctx);
  // end::ctx_setup_source[]
  std::unique_ptr<ssl::stream<tcp::socket>> streamPtr =
      connect(ctx, ssl_ctx, hostname);

  // response
  responseBeast = std::make_shared<
      boost::beast::http::response<boost::beast::http::string_body>>();
  responsePtr = std::make_shared<ResponseImpl>();
  responsePtr->setResponse(responseBeast);

  if (method == "POST") {
    requestBeast->method(http::verb::post);
  } else {
    requestBeast->method(http::verb::get);
  }
  requestBeast->target(urlPtr->target());
  requestBeast->keep_alive(false);
  requestBeast->set(http::field::host, hostname);

#if BOOKFILER_HTTP_CLIENT_END_DEBUG_RESPONSE
  std::cout << "\n=== THREAD " << threadId << " ===\n"
            << moduleCode << "::ClientImpl::end request:\n"
            << *requestBeast << std::endl;
#endif

  http::write(*streamPtr, *requestBeast);

  beast::flat_buffer buffer;
  http::read(*streamPtr, buffer, *responseBeast);

  if (responseBeast->result() == boost::beast::http::status::ok) {
    responseStr = responseBeast->body();
    // Try to parse the response as JSON
    responseJsonDoc = std::make_shared<rapidjson::Document>();
    rapidjson::ParseResult ok =
        responseJsonDoc->Parse(responseBeast->body().c_str());
    if (!ok) {
      responseJsonDoc.reset();
    }
  }

#if BOOKFILER_HTTP_CLIENT_END_DEBUG_RESPONSE
  std::cout << "\n=== THREAD " << threadId << " ===\n"
            << moduleCode << "::ClientImpl::end response:\n"
            << *responseBeast << std::endl;
#endif

  boost::system::error_code ec;
  streamPtr->shutdown(ec);
  streamPtr->next_layer().close(ec);

  return 0;
}

int ClientImpl::endAsync() { return 0; }
int ClientImpl::wait() { return 0; }

} // namespace HTTP
} // namespace bookfiler
