/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "Server.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

ServerImpl::ServerImpl() {
  routePtr = std::make_shared<RouteImpl>();
  // The SSL context holds certificates
  sslContext = std::make_shared<boost::asio::ssl::context>(
      boost::asio::ssl::context::tlsv12);
  serverState = std::make_shared<ServerState>();
}

ServerImpl::~ServerImpl() {
  for (auto &thread : threadList) {
    thread.join();
  }
}

int ServerImpl::setSettingsDoc(std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}

int ServerImpl::run() {
  extractSettings();
  std::stringstream ss;
  auto hardwareThreadsNum = std::thread::hardware_concurrency();
  ss << "threadsNum: " << serverState->threadsNum
     << "\nHardware Threads: " << hardwareThreadsNum;
  serverState->threadsNum = hardwareThreadsNum;
  logStatus("::ServerImpl::runAsync", ss.str());
  // hint how much concurrency is used for the io context
  ioContext =
      std::make_shared<boost::asio::io_context>(serverState->threadsNum);
  // Create and launch a listening port
  net::spawn(
      *ioContext,
      std::bind(&Listener::run,
                std::make_shared<Listener>(
                    std::ref(*ioContext), std::ref(*sslContext),
                    tcp::endpoint{serverState->address, serverState->port},
                    serverState, routePtr),
                std::placeholders::_1));

  // Run the I/O service on the requested number of threads
  threadList.reserve(serverState->threadsNum);
  for (auto i = serverState->threadsNum; i > 0; --i) {
    threadList.emplace_back(&ServerImpl::runIoContext, this);
  }

  return 0;
}

void ServerImpl::runIoContext() { ioContext->run(); }

int ServerImpl::extractSettings() {
  /* Get settings from JSON document */
  if (!settingsDoc->IsObject()) {
    logStatus("::ServerImpl::extractSettings",
              "ERROR: Settings document not an object.");
    return -1;
  }
  if (!settingsDoc->HasMember("server")) {
    logStatus("::ServerImpl::extractSettings",
              "ERROR: Settings document has no server definition.");
    return -1;
  }
  const rapidjson::Value &ServerStateJson = (*settingsDoc)["server"];

  if (!ServerStateJson.IsObject()) {
    logStatus("::ServerImpl::extractSettings",
              "ERROR: Settings document server member is not an object.");
    return -1;
  }

  auto addressStrOpt =
      bookfiler::JSON::getMemberString(ServerStateJson, "address");
  if (addressStrOpt) {
    serverState->addressStr = *addressStrOpt;
  }
  auto docRootStrOpt =
      bookfiler::JSON::getMemberString(ServerStateJson, "docRoot");
  if (docRootStrOpt) {
    serverState->docRootStr = *docRootStrOpt;
  }
  auto portIntOpt = bookfiler::JSON::getMemberInt(ServerStateJson, "port");
  if (portIntOpt) {
    serverState->portInt = *portIntOpt;
  }
  auto threadsNumOpt =
      bookfiler::JSON::getMemberInt(ServerStateJson, "threadNum");
  if (threadsNumOpt) {
    serverState->threadsNum = *threadsNumOpt;
  }

  serverState->address = boost::asio::ip::make_address(serverState->addressStr);
  serverState->port = static_cast<unsigned short>(serverState->portInt);
  serverState->threadsNum = std::max<int>(1, serverState->threadsNum);
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
    std::map<std::string, routeVariantTypeExternal> map_) {
  logStatus("::ServerImpl::route", "START");
  std::shared_ptr<routeFunctionTypeExternal> routeFunction;
  std::string method, path;
  int priority = 10;
  method = "GET";
  path = "*";
  for (auto val : map_) {
    if (int *val_ = std::get_if<int>(&val.second)) {
      if (val.first == "priority") {
        priority = *val_;
      }
    } else if (double *val_ = std::get_if<double>(&val.second)) {
    } else if (std::string *val_ = std::get_if<std::string>(&val.second)) {
      if (val.first == "method") {
        method = *val_;
      } else if (val.first == "path") {
        path = *val_;
      }
    } else if (routeFunctionTypeExternal *val_ =
                   std::get_if<routeFunctionTypeExternal>(&val.second)) {
      routeFunction =
          std::make_shared<routeFunctionTypeExternal>(std::move(*val_));
    }
  }
  if (method == "GET") {
    routePtr->getAdd(path, priority, *routeFunction);
  } else if (method == "POST") {
    routePtr->postAdd(path, priority, *routeFunction);
  } else if (method == "*") {
    routePtr->allAdd(path, priority, *routeFunction);
  }

  return 0;
}

} // namespace HTTP
} // namespace bookfiler
