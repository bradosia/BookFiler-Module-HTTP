/*
 * @name BookFiler Module Loader
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief simple module loader for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_LOADER_H
#define BOOKFILER_MODULE_HTTP_LOADER_H

// c++17
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>

// bradosia libraries 1.0
#include <ModuleManager/ModuleManager.hpp>
#include <SettingsManager/SettingsManager.hpp>

// Modules
#include <BookFiler-Module-HTTP/Interface.hpp>

std::shared_ptr<bradosia::ModuleManager> moduleManagerPtr;
std::shared_ptr<bradosia::SettingsManager> settingsManagerPtr;

/*
 * bookfiler - Curl
 */
namespace bookfiler {
namespace HTTP {

int loadModule(std::string moduleDirPath, std::function<void()> callback,
               std::shared_ptr<bookfiler::HTTP::ModuleInterface> &module) {
#if loadModules_DEBUG
  std::cout << "loadModules() BEGIN\n";
#endif
  settingsManagerPtr = std::make_shared<bradosia::SettingsManager>();
  /* Module Load
   */
  moduleManagerPtr = std::make_shared<bradosia::ModuleManager>();
  moduleManagerPtr->addModule<bookfiler::HTTP::ModuleInterface>(
      "bookfilerHttpModule");
  moduleManagerPtr->getCallbackLoadSignal<bookfiler::HTTP::ModuleInterface>(
      "bookfilerHttpModule")
      ->connect([&module](
                    std::shared_ptr<bookfiler::HTTP::ModuleInterface> module_)
                    -> int {
        module = module_;
        /* register widgets
         */
        module->init();
        /* register setting deploy
         */
        std::shared_ptr<rapidjson::Document> moduleRequest =
            std::make_shared<rapidjson::Document>();
        std::shared_ptr<std::unordered_map<
            std::string,
            std::function<void(std::shared_ptr<rapidjson::Document>)>>>
            moduleCallbackMap = std::make_shared<std::unordered_map<
                std::string,
                std::function<void(std::shared_ptr<rapidjson::Document>)>>>();
        module->registerSettings(moduleRequest, moduleCallbackMap);
        settingsManagerPtr->merge(moduleRequest, moduleCallbackMap);
        return 0;
      });
  moduleManagerPtr->callbackLoadAllSignal.connect([&callback]() -> int {
    /* Get the settings
     */
    settingsManagerPtr->deployFile("settings.json");
    callback();
    return 0;
  });
  moduleManagerPtr->loadModules(moduleDirPath);
#if loadModules_DEBUG
  std::cout << "loadModules() END\n";
#endif
  return 0;
}

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_INTERFACE_H
