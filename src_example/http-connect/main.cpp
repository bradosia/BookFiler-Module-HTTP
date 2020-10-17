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
int allModulesLoaded();
int jsonReceived(std::shared_ptr<rapidjson::Document>);

std::string testName = "HTTP Connection Test";
std::shared_ptr<bookfiler::HTTP::ModuleInterface> mySQL_Module;
std::shared_ptr<bradosia::ModuleManager> moduleManagerPtr;
std::shared_ptr<bradosia::SettingsManager> settingsManagerPtr;

int main() {
  std::cout << testName << " BEGIN" << std::endl;

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
  moduleManagerPtr->addModule<bookfiler::HTTP::ModuleInterface>("mySQLModule");
  moduleManagerPtr
      ->getCallbackLoadSignal<bookfiler::HTTP::ModuleInterface>("mySQLModule")
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

int allModulesLoaded() {
  int rc;

  /* Get the settings
   */
  settingsManagerPtr->deployFile(SETTINGS_FILE);

  /* Example using the module */
  bookfiler::curl::Init initObj;

  std::shared_ptr<bookfiler::HTTP::Connection> HTTP_Connection =
      mySQL_Module->newConnection();
  rc = HTTP_Connection->setURL(
      "http://data.nba.net/prod/v1/20170201/0021600732_boxscore.json");
  HTTP_Connection->jsonReceivedSignal.connect(
      std::bind(&jsonReceived, std::placeholders::_1));
  HTTP_Connection->setMethod("GET");
  rc = HTTP_Connection->exec();
  if (rc < 0) {
    std::cout << "Could not access webpage by HTTP\n";
    return -1;
  }

  return 0;
}

int jsonReceived(std::shared_ptr<rapidjson::Document> jsonDoc) {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  jsonDoc->Accept(writer);
  std::cout << "jsonReceived:\n"
            << buffer.GetString() << std::endl;
  return 0;
}
