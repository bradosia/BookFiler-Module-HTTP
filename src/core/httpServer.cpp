/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServer.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

ServerImpl::ServerImpl() {
  routeSignal = std::make_shared<routeSignalTypeInternal>();
  // The SSL context holds certificates
  sslContext = std::make_shared<boost::asio::ssl::context>(
      boost::asio::ssl::context::tlsv12);
}

ServerImpl::~ServerImpl() {}

int ServerImpl::setSettingsDoc(std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}

int ServerImpl::run() {
  extractSettings();
  // hint how much concurrency is used for the io context
  ioContext = std::make_shared<boost::asio::io_context>(threadsNum);
  // Create and launch a listening port
  listener = std::make_shared<Listener>(*ioContext, *sslContext,
                                        tcp::endpoint{address, port}, docRoot);
  listener->setRouteSignal(routeSignal);
  listener->run();

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(threadsNum - 1);
  for (auto i = threadsNum - 1; i > 0; --i) {
    v.emplace_back(&ServerImpl::runIoContext, this);
    v.back().detach();
  }
  ioContext->run();

  return 0;
}

int ServerImpl::runAsync() {
  extractSettings();
  // hint how much concurrency is used for the io context
  ioContext = std::make_shared<boost::asio::io_context>(threadsNum);
  // Create and launch a listening port
  listener = std::make_shared<Listener>(*ioContext, *sslContext,
                                        tcp::endpoint{address, port}, docRoot);
  listener->setRouteSignal(routeSignal);
  listener->run();

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(threadsNum);
  for (auto i = threadsNum; i > 0; --i) {
    v.emplace_back(&ServerImpl::runIoContext, this);
    v.back().detach();
  }

  return 0;
}

void ServerImpl::runIoContext() { ioContext->run(); }

int ServerImpl::extractSettings() {
  std::string addressStr, docRootStr;
  int portInt = 0;

  /* Get settings from JSON document */
  if (!settingsDoc->IsObject()) {
    std::cout << moduleCode
              << "::ServerImpl::run ERROR:\nSettings document invalid"
              << std::endl;
    return -1;
  }
  if (!settingsDoc->HasMember("server")) {
    std::cout << moduleCode
              << "::ServerImpl::run ERROR:\nSettings document has no server "
                 "definition."
              << std::endl;
    return -1;
  }
  const rapidjson::Value &serverSettingsJson = (*settingsDoc)["server"];

  if (!serverSettingsJson.IsObject()) {
    std::cout << moduleCode
              << "::ServerImpl::run ERROR:\nSettings document server "
                 "definition incorrect type."
              << std::endl;
    return -1;
  }

  char memberName01[] = "address";
  if (serverSettingsJson.HasMember(memberName01) &&
      serverSettingsJson[memberName01].IsString()) {
    addressStr = serverSettingsJson[memberName01].GetString();
  }
  char memberName02[] = "port";
  if (serverSettingsJson.HasMember(memberName02) &&
      serverSettingsJson[memberName02].IsInt()) {
    portInt = serverSettingsJson[memberName02].GetInt();
  }
  char memberName03[] = "docRoot";
  if (serverSettingsJson.HasMember(memberName03) &&
      serverSettingsJson[memberName03].IsString()) {
    docRootStr = serverSettingsJson[memberName03].GetString();
  }
  char memberName04[] = "threadNum";
  if (serverSettingsJson.HasMember(memberName04) &&
      serverSettingsJson[memberName04].IsInt()) {
    threadsNum = serverSettingsJson[memberName04].GetInt();
  }

  address = boost::asio::ip::make_address(addressStr);
  port = static_cast<unsigned short>(portInt);
  docRoot = std::make_shared<std::string>(docRootStr);
  threadsNum = std::max<int>(1, threadsNum);
  return 0;
}

int ServerImpl::useCertificate(
    std::shared_ptr<bookfiler::certificate::Certificate> certPtr) {
  std::shared_ptr<bookfiler::certificate::CertificateImpl> certImplPtr =
      std::static_pointer_cast<bookfiler::certificate::CertificateImpl>(
          certPtr);

  std::shared_ptr<std::string> certStr = certImplPtr->getCertStr();
  if (!certStr) {
    std::cout << moduleCode
              << "::ServerImpl::useCertificate ERROR:\nCould not get cert."
              << std::endl;
    return -1;
  }

  std::shared_ptr<std::string> privateKeyStr = certImplPtr->getPrivateKeyStr();
  if (!privateKeyStr) {
    std::cout << moduleCode
              << "::ServerImpl::useCertificate ERROR:\nCould not get cert."
              << std::endl;
    return -1;
  }

  std::shared_ptr<std::string> dhStr = certImplPtr->getDhStr();
  if (!dhStr) {
    std::cout << moduleCode
              << "::ServerImpl::useCertificate ERROR:\nCould not get cert."
              << std::endl;
    return -1;
  }

  sslContext->set_password_callback(
      [](std::size_t, boost::asio::ssl::context_base::password_purpose) {
        return "test";
      });

  sslContext->set_options(boost::asio::ssl::context::default_workarounds |
                          boost::asio::ssl::context::no_sslv2 |
                          boost::asio::ssl::context::single_dh_use);

  sslContext->use_certificate_chain(
      boost::asio::buffer(certStr->data(), certStr->size()));

  sslContext->use_private_key(
      boost::asio::buffer(privateKeyStr->data(), privateKeyStr->size()),
      boost::asio::ssl::context::file_format::pem);

  sslContext->use_tmp_dh(boost::asio::buffer(dhStr->data(), dhStr->size()));

  return 0;
}

int ServerImpl::route(
    std::unordered_map<std::string, routeVariantType_Beast> map) {
  std::cout << "routeVariantType_Beast" << std::endl;
  std::shared_ptr<routeFunctionExpressType> routeFunctionExpress;
  std::string method, path;
  for (auto val : map) {
    if (int *val_ = std::get_if<int>(&val.second)) {
    } else if (double *val_ = std::get_if<double>(&val.second)) {
    } else if (std::string *val_ = std::get_if<std::string>(&val.second)) {
      if (val.first == "method") {
        method = *val_;
      } else if (val.first == "path") {
        path = *val_;
      }
    } else if (routeFunctionExpressType *val_ =
                   std::get_if<routeFunctionExpressType>(&val.second)) {
      routeFunctionExpress =
          std::make_shared<routeFunctionExpressType>(std::move(*val_));
    } else if (routeFunctionBeastTypeInternal *val_ =
                   std::get_if<routeFunctionBeastTypeInternal>(&val.second)) {
      routeFunctionBeastTypeInternal callback = std::move(*val_);
      std::cout << "routeFunctionBeastType added" << std::endl;
      routeSignal->connect(callback);
    }
  }

  /* converts from an Express-like callback to the native Beast callback */
  routeSignalAdd(routeFunctionExpress, method, path);

  return 0;
}

int ServerImpl::route(
    std::unordered_map<std::string, routeVariantType_NoBeast> map) {
  std::cout << "routeVariantType_NoBeast" << std::endl;
  std::shared_ptr<routeFunctionExpressType> routeFunctionExpress;
  std::string method, path;
  for (auto val : map) {
    if (int *val_ = std::get_if<int>(&val.second)) {
    } else if (double *val_ = std::get_if<double>(&val.second)) {
    } else if (std::string *val_ = std::get_if<std::string>(&val.second)) {
      if (val.first == "method") {
        method = *val_;
      } else if (val.first == "path") {
        path = *val_;
      }
    } else if (routeFunctionExpressType *val_ =
                   std::get_if<routeFunctionExpressType>(&val.second)) {
      routeFunctionExpress =
          std::make_shared<routeFunctionExpressType>(std::move(*val_));
    }
  }

  /* converts from an Express-like callback to the native Beast callback */
  routeSignalAdd(routeFunctionExpress, method, path);

  return 0;
}

int ServerImpl::routeSignalAdd(
    std::shared_ptr<routeFunctionExpressType> routeFunctionExpress,
    std::string method, std::string path) {
  std::cout << "rsdfsdfsdfsdfsdf" << std::endl;
  routeSignal->connect([routeFunctionExpress, method,
                        path](std::shared_ptr<Session> session,
                              requestBeastInternal reqBeast,
                              responseBeastInternal resBeast) -> int {
    std::cout << "routeSignal->connect" << std::endl;
    std::shared_ptr<Request> req = session->parseRequest(reqBeast);
    if (session->routeValidate(req, method, path) < 0) {
      return -1;
    }
    std::shared_ptr<ResponseImpl> resImpl = std::make_shared<ResponseImpl>();
    resImpl->setResponse(resBeast);
    response res = std::dynamic_pointer_cast<Response>(resImpl);
    std::string returnStr = (*routeFunctionExpress)(req, res);
    resBeast->result(boost::beast::http::status::ok);
    resBeast->version(reqBeast->version());
    resBeast->set(boost::beast::http::field::server,
                  BOOST_BEAST_VERSION_STRING);
    resBeast->set(boost::beast::http::field::content_type, "text/html");
    resBeast->keep_alive(reqBeast->keep_alive());
    resBeast->body() = returnStr;
    resBeast->content_length(resBeast->body().length());
    resBeast->prepare_payload();
    return 0;
  });
  return 0;
}

std::shared_ptr<routeSignalTypeInternal> ServerImpl::getRouteSignal() {
  return routeSignal;
}

} // namespace HTTP
} // namespace bookfiler
