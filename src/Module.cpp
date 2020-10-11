/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/prettywriter.h>

// Local Project
#include "Module.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

int ModuleExport::init() {
  std::cout << moduleName << ": init()" << std::endl;
  return 0;
}

int ModuleExport::registerSettings(
    std::shared_ptr<rapidjson::Document> moduleRequest,
    std::shared_ptr<std::unordered_map<
        std::string, std::function<void(std::shared_ptr<rapidjson::Document>)>>>
        moduleCallbackMap) {
  moduleRequest->SetObject();
  moduleRequest->AddMember("HTTP_accounts", "HTTP_accountsCB",
                           moduleRequest->GetAllocator());
  moduleCallbackMap->insert(
      {"HTTP_accountsCB",
       std::bind(&ModuleExport::setAccounts, this, std::placeholders::_1)});
  moduleRequest->AddMember("HTTP_settings", "HTTP_settingsCB",
                           moduleRequest->GetAllocator());
  moduleCallbackMap->insert(
      {"HTTP_settingsCB",
       std::bind(&ModuleExport::setSettings, this, std::placeholders::_1)});
  return 0;
}

int ModuleExport::setAccounts(std::shared_ptr<rapidjson::Value> jsonDoc) {
  accountsDoc = jsonDoc;
#if MODULE_EXPORT_SET_ACCOUNTS_DEBUG
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  jsonDoc->Accept(writer);
  std::cout << moduleCode << "::ModuleExport::setAccounts:\n"
            << buffer.GetString() << std::endl;
#endif
  return 0;
}

int ModuleExport::setSettings(std::shared_ptr<rapidjson::Value> jsonDoc) {
  settingsDoc = jsonDoc;
#if MODULE_EXPORT_SET_SETTINGS_DEBUG
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  jsonDoc->Accept(writer);
  std::cout << moduleCode << "::ModuleExport::setSettings:\n"
            << buffer.GetString() << std::endl;
#endif
  return 0;
}

std::shared_ptr<Connection> ModuleExport::newConnection() {
  std::shared_ptr<ConnectionImpl> connectionPtr =
      std::make_shared<ConnectionImpl>();
  connectionPtr->setSettingsDoc(settingsDoc);
  // connectionPtr->setAccountsDoc(accountsDoc);
  return std::dynamic_pointer_cast<Connection>(connectionPtr);
}

std::shared_ptr<Url> ModuleExport::newUrl() {
  std::shared_ptr<UrlImpl> urlPtr = std::make_shared<UrlImpl>();
  return std::dynamic_pointer_cast<Url>(urlPtr);
}

} // namespace HTTP
} // namespace bookfiler
