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
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>

// Bookfiler Modules
#include <BookFilerModuleHttpLoader.hpp>

int allModulesLoaded();
int jsonReceived(std::shared_ptr<rapidjson::Document>);

std::string testName = "HTTP Client Example";
std::shared_ptr<bookfiler::HTTP::ModuleInterface> httpModule;

int main() {
  std::cout << testName << " BEGIN" << std::endl;

  bookfiler::curl::Init initObj;
  bookfiler::HTTP::loadModule("modules", std::bind(&allModulesLoaded),
                              httpModule);

  std::cout << testName << " END" << std::endl;
  system("pause");
  return 0;
}

int allModulesLoaded() {
  int rc = 0;
  /* Example using the module */
  bookfiler::curl::Init initObj;

  std::shared_ptr<bookfiler::HTTP::Client> httpClient = httpModule->newClient({
      {"host", "data.nba.net"},
      {"path", "/prod/v1/20170201/0021600732_boxscore.json"},
      {"method", "GET"},
  });
  rc = httpClient->setURL(
      "http://data.nba.net/prod/v1/20170201/0021600732_boxscore.json");
  httpClient->jsonReceivedSignal.connect(
      std::bind(&jsonReceived, std::placeholders::_1));
  httpClient->setMethod("GET");
  rc = httpClient->exec();
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
  std::cout << "jsonReceived:\n" << buffer.GetString() << std::endl;
  return 0;
}
