/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#define loadModules_DEBUG 1
#define SETTINGS_FILE "settings.json"

// C++17
#include <iostream>
#include <memory>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>

// bradosia libraries 1.0
#include <ModuleManager/ModuleManager.hpp>
#include <SettingsManager/SettingsManager.hpp>

// Modules
#include <BookFiler-Module-HTTP/Interface.hpp>

// Local Project
// #include ""

int loadModules();
int moduleLoaded(std::shared_ptr<bookfiler::HTTP::ModuleInterface>);
int routeSlot(
    std::shared_ptr<rapidjson::Document>,
    std::shared_ptr<
        boost::beast::http::request<boost::beast::http::string_body>>,
    std::shared_ptr<
        boost::beast::http::response<boost::beast::http::string_body>>);
int allModulesLoaded();

std::string testName = "HTTP Server Test";
std::shared_ptr<bookfiler::HTTP::ModuleInterface> mySQL_Module;
std::shared_ptr<bradosia::ModuleManager> moduleManagerPtr;
std::shared_ptr<bradosia::SettingsManager> settingsManagerPtr;

int main() {
  std::cout << testName << " BEGIN" << std::endl;

  bookfiler::curl::Init initObj;
  loadModules();

  std::cout << testName << " END" << std::endl;
  system("pause");
  return 0;
}

int loadModules() {
#if loadModules_DEBUG
  std::cout << "loadModules() BEGIN\n";
#endif
  settingsManagerPtr = std::make_shared<bradosia::SettingsManager>();
  /* Module Load
   */
  moduleManagerPtr = std::make_shared<bradosia::ModuleManager>();
  moduleManagerPtr->addModule<bookfiler::HTTP::ModuleInterface>(
      "bookfilerHttpModule");
  moduleManagerPtr
      ->getCallbackLoadSignal<bookfiler::HTTP::ModuleInterface>(
          "bookfilerHttpModule")
      ->connect(std::bind(&moduleLoaded, std::placeholders::_1));
  moduleManagerPtr->callbackLoadAllSignal.connect(std::bind(&allModulesLoaded));
  moduleManagerPtr->loadModules("modules");
#if loadModules_DEBUG
  std::cout << "loadModules() END\n";
#endif
  return 0;
}

int moduleLoaded(std::shared_ptr<bookfiler::HTTP::ModuleInterface> module) {
  mySQL_Module = module;
  /* register widgets
   */
  mySQL_Module->init();
  /* register setting deploy
   */
  std::shared_ptr<rapidjson::Document> moduleRequest =
      std::make_shared<rapidjson::Document>();
  std::shared_ptr<std::unordered_map<
      std::string, std::function<void(std::shared_ptr<rapidjson::Document>)>>>
      moduleCallbackMap = std::make_shared<std::unordered_map<
          std::string,
          std::function<void(std::shared_ptr<rapidjson::Document>)>>>();
  mySQL_Module->registerSettings(moduleRequest, moduleCallbackMap);
  settingsManagerPtr->merge(moduleRequest, moduleCallbackMap);
  return 0;
}

int routeSlot(std::shared_ptr<rapidjson::Document> sessionDocument,
              std::shared_ptr<
                  boost::beast::http::request<boost::beast::http::string_body>>
                  req,
              std::shared_ptr<
                  boost::beast::http::response<boost::beast::http::string_body>>
                  res) {
  std::string bodyStr = "<h1>URL Data</h1><br>";
  bodyStr.append(req->target().data());

  res->result(boost::beast::http::status::ok);
  res->version(req->version());
  res->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  res->set(boost::beast::http::field::content_type, "text/html");
  res->keep_alive(req->keep_alive());
  res->body() = bodyStr;
  res->content_length(res->body().length());
  res->prepare_payload();
  return 0;
}

int allModulesLoaded() {
  int rc;

  /* Get the settings
   */
  settingsManagerPtr->deployFile(SETTINGS_FILE);

  /* Start using the module
   * Create an HTTP server for the authorization page to redirect to so that the
   * authorization code can be captured.
   */

  std::shared_ptr<bookfiler::certificate::Manager> certificateManager =
      mySQL_Module->newCertificateManager();
  std::shared_ptr<bookfiler::certificate::Certificate> certRootPtr,
      certServerPtr;
  // certificateManager->newCertRootLocalhost(certRootPtr, nullptr);
  certificateManager->loadCertificate(certRootPtr);
  if (certRootPtr) {
    std::cout << "root Certificate Info: " << certRootPtr->getInfo()
              << std::endl;
    // certificateManager->saveCertificate(certRootPtr,"root");
    // certificateManager->addCertificate(certRootPtr);
    // certificateManager->newCertServerLocalhost(certServerPtr, nullptr);
    if (certServerPtr) {
      std::cout << "Server Certificate Info: " << certServerPtr->getInfo()
                << std::endl;
      certificateManager->createX509Store();
    }
  }

  // start server
  std::shared_ptr<bookfiler::HTTP::Server> httpServer =
      mySQL_Module->newServer();
  httpServer->useCertificate(certRootPtr);
  httpServer->runAsync();
  httpServer->routeSignal->connect(&routeSlot);
  httpServer->route({{"key", "value"}, {"key", 4}});
  httpServer->route({{"key", "value"}, {"key", true}, {"key", 6.999}});

  system("pause");
  return 0;
}
