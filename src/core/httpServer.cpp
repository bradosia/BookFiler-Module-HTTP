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

ServerImpl::ServerImpl() { routeSignal = std::make_shared<routeSignalType>(); }

ServerImpl::~ServerImpl() {}

int ServerImpl::setSettingsDoc(std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}

int ServerImpl::run() {
  extractSettings();
  // hint how much concurrency is used for the io context
  ioContext = std::make_shared<boost::asio::io_context>(threadsNum);
  // The SSL context is required, and holds certificates
  sslContext = std::make_shared<boost::asio::ssl::context>(
      boost::asio::ssl::context::tlsv12);
  // This holds the self-signed certificate used by the server
  loadServerCertificate(sslContext);
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
  // The SSL context is required, and holds certificates
  sslContext = std::make_shared<boost::asio::ssl::context>(
      boost::asio::ssl::context::tlsv12);
  // This holds the self-signed certificate used by the server
  loadServerCertificate(sslContext);
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

} // namespace HTTP
} // namespace bookfiler
