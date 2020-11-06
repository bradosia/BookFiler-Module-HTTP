/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// C++
#include <thread>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>

// Bookfiler Modules
#include <BookFilerModuleHttpLoader.hpp>

int allModulesLoaded();

std::string testName = "HTTP Client Example";
std::shared_ptr<bookfiler::HTTP::ModuleInterface> httpModule;

int main() {
  std::cout << testName << " BEGIN" << std::endl;

  bookfiler::HTTP::loadModule("modules", std::bind(&allModulesLoaded),
                              httpModule);

  std::cout << testName << " END" << std::endl;
  system("pause");
  return 0;
}

int allModulesLoaded() {
  int rc = 0;
  /* Example using the module */
  std::shared_ptr<bookfiler::HTTP::Client> httpClient = httpModule->newClient({
      {"host", "data.nba.net"},
      {"path", "/prod/v1/20170201/0021600732_boxscore.json"},
      {"scheme", "http"},
      {"method", "GET"},
  });
  httpClient->setMethod("GET");
  rc = httpClient->end();
  if (rc < 0) {
    std::cout << "Could not access webpage by HTTP\n";
    return -1;
  }

  std::optional<std::shared_ptr<rapidjson::Document>> jsonDocOpt =
      httpClient->getResponseJson();

  if (!jsonDocOpt) {
    std::cout << "JSON response not valid\n";
    return 0;
  }

  std::shared_ptr<rapidjson::Document> &jsonDoc = *jsonDocOpt;

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  jsonDoc->Accept(writer);

  std::thread::id threadId = std::this_thread::get_id();
  std::cout << "\n=== THREAD " << threadId << " ===\n"
            << testName << " allModulesLoaded jsonDoc PrettyWriter:\n"
            << buffer.GetString() << std::endl;

  return 0;
}
