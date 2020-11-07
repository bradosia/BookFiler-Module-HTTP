/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
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

std::shared_ptr<Client> ModuleExport::newClient() {
  std::shared_ptr<ClientImpl> connectionPtr = std::make_shared<ClientImpl>();
  connectionPtr->setSettingsDoc(settingsDoc);
  // connectionPtr->setAccountsDoc(accountsDoc);
  return std::dynamic_pointer_cast<Client>(connectionPtr);
}

std::shared_ptr<Client>
ModuleExport::newClient(std::map<std::string, newClientVariantType> map) {
  std::shared_ptr<ClientImpl> connectionPtr = std::make_shared<ClientImpl>(map);
  connectionPtr->setSettingsDoc(settingsDoc);
  // connectionPtr->setAccountsDoc(accountsDoc);
  return std::dynamic_pointer_cast<Client>(connectionPtr);
}

std::shared_ptr<Url>
ModuleExport::newUrl(std::map<std::string, newUrlVariantType> map_) {
  std::shared_ptr<UrlImpl> urlPtr = std::make_shared<UrlImpl>(map_);
  return std::dynamic_pointer_cast<Url>(urlPtr);
}

std::shared_ptr<Server>
ModuleExport::newServer(std::map<std::string, newServerVariantType> map) {
  std::shared_ptr<ServerImpl> serverPtr = std::make_shared<ServerImpl>();
  serverPtr->setSettingsDoc(settingsDoc);
  return std::dynamic_pointer_cast<Server>(serverPtr);
}

std::shared_ptr<bookfiler::certificate::Manager>
ModuleExport::newCertificateManager() {
  std::shared_ptr<bookfiler::certificate::ManagerNativeImpl> managerPtr =
      std::make_shared<bookfiler::certificate::ManagerNativeImpl>();
  managerPtr->setSettingsDoc(settingsDoc);
  return std::dynamic_pointer_cast<bookfiler::certificate::Manager>(managerPtr);
}

void ModuleExport::wait(const std::string handle_) {
  std::unique_lock<std::mutex> mutexLock(globalMutex);
  conditionVariableMap.emplace(std::piecewise_construct,
                               std::forward_as_tuple(handle_), // args for key
                               std::forward_as_tuple());
  auto it = conditionVariableMap.find(handle_);
  if (it != conditionVariableMap.end()) {
    conditionVariableMap.at(handle_).wait(mutexLock);
  }
}

void ModuleExport::notify(const std::string handle_) {
  auto it = conditionVariableMap.find(handle_);
  if (it != conditionVariableMap.end()) {
    conditionVariableMap.at(handle_).notify_one();
  }
}

} // namespace HTTP
} // namespace bookfiler
