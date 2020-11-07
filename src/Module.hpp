/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_EXPORT_H
#define BOOKFILER_MODULE_HTTP_EXPORT_H

// config
#include "core/config.hpp"

// c++17
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <map>

/* bustache 0.1.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <bustache/model.hpp>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/reader.h> // rapidjson::ParseResult
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

// Local Project
//#include "core/certificateManager.hpp"
#include "core/Client.hpp"
#include "core/Server.hpp"
#include "core/json.hpp"

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

/*
 * This widget uses the MVC design pattern.
 * This is the controller, the view is a QT widget, and the model is the API
 * storage
 */
class ModuleExport : public ModuleInterface {
private:
  std::shared_ptr<rapidjson::Value> settingsDoc;
  std::shared_ptr<rapidjson::Value> accountsDoc;
  std::map<std::string, std::condition_variable> conditionVariableMap;

public:
  ModuleExport(){};
  ~ModuleExport(){};

  /* module typical methods
   * init, registerSettings
   */
  int init();
  /* registerSettings
   * @brief Registers a map of callbacks for handling data from the main
   * application settings file.
   */
  int registerSettings(
      std::shared_ptr<rapidjson::Document>,
      std::shared_ptr<std::unordered_map<
          std::string,
          std::function<void(std::shared_ptr<rapidjson::Document>)>>>);
  int setAccounts(std::shared_ptr<rapidjson::Value>);
  int setSettings(std::shared_ptr<rapidjson::Value>);
  std::shared_ptr<Client> newClient();
  std::shared_ptr<Client>
      newClient(std::map<std::string, newClientVariantType>);
  std::shared_ptr<Url>
      newUrl(std::map<std::string, newUrlVariantType>);
  std::shared_ptr<Server>
      newServer(std::map<std::string, newServerVariantType>);
  std::shared_ptr<bookfiler::certificate::Manager> newCertificateManager();
  // Not very related, but good for the examples
  void wait(const std::string handle_);
  void notify(const std::string handle_);
};

// Exporting `my_namespace::module` variable with alias name `module`
// (Has the same effect as `BOOST_DLL_ALIAS(my_namespace::module, module)`)
extern "C" BOOST_SYMBOL_EXPORT ModuleExport bookfilerHttpModule;
ModuleExport bookfilerHttpModule;

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_EXPORT_H
