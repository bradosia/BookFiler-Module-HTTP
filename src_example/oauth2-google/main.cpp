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
int jsonReceived(std::shared_ptr<rapidjson::Document>);
int apiJsonReceived(std::shared_ptr<rapidjson::Document>);

std::string testName = "HTTP Connection Test";
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
  certificateManager->newCertRootLocalhost(certRootPtr, nullptr);
  if (certRootPtr) {
    std::cout << "root Certificate Info: " << certRootPtr->getInfo()
              << std::endl;
    certificateManager->saveCertificate(certRootPtr,"root");
    certificateManager->addCertificate(certRootPtr);
    certificateManager->newCertServerLocalhost(certServerPtr, nullptr);
    if (certServerPtr) {
      std::cout << "Server Certificate Info: " << certServerPtr->getInfo()
                << std::endl;
      certificateManager->createX509Store();
    }
  }

  // start server
  std::shared_ptr<bookfiler::HTTP::Server> httpServer =
      mySQL_Module->newServer();
  httpServer->useCertificate(certServerPtr);
  httpServer->runAsync();
  httpServer->routeSignal->connect(&routeSlot);

  // authorization url
  std::shared_ptr<bookfiler::HTTP::Url> authUrl = mySQL_Module->newUrl();
  authUrl->setBase("https://accounts.google.com/o/oauth2/v2/auth");
  std::shared_ptr<std::unordered_map<std::string, std::string>> fieldsMap =
      std::make_shared<std::unordered_map<std::string, std::string>>();
  fieldsMap->insert({"client_id",
                     "854776203850-r64s69l8jmh71ugiio16impqfcp80j1m.apps."
                     "googleusercontent.com"});
  fieldsMap->insert({"scope", "https://mail.google.com/"});
  fieldsMap->insert({"response_type", "code"});
  fieldsMap->insert({"redirect_uri", "https://localhost:8082"});
  authUrl->setFields(fieldsMap);

  boost::url authUrl2;
  authUrl2.set_scheme("https");
  authUrl2.set_host("accounts.google.com/o/oauth2/v2/auth");
  std::string fieldsStr = authUrl->getFieldsStr();
  authUrl2.set_query(fieldsStr);
  std::cout << authUrl2.encoded_url() << "\n\n";

  // open google oauth2 window
  std::string windowOpenCommand = "explorer \"";
  windowOpenCommand.append(authUrl->getURL()).append("\"");
  system(windowOpenCommand.c_str());

  std::string auth_code;
  std::cout << "Please login to your google account and paste in the "
               "authorization code.\nauthorization code:";
  std::getline(std::cin, auth_code);
  std::cout << "\nAuthorization Code: " << auth_code << "\n";

  /* Start an HTTP post request to get the access token
   */
  std::shared_ptr<bookfiler::HTTP::Connection> HTTP_Connection =
      mySQL_Module->newConnection();

  // token url
  std::shared_ptr<std::unordered_map<std::string, std::string>> tokenFieldsMap =
      std::make_shared<std::unordered_map<std::string, std::string>>();
  tokenFieldsMap->insert({"client_id",
                          "854776203850-r64s69l8jmh71ugiio16impqfcp80j1m.apps."
                          "googleusercontent.com"});
  tokenFieldsMap->insert({"client_secret", "18e_rRNMBIJOJ0jWUthY7RKp"});
  tokenFieldsMap->insert({"grant_type", "authorization_code"});
  tokenFieldsMap->insert({"redirect_uri", "urn:ietf:wg:oauth:2.0:oob"});
  tokenFieldsMap->insert({"code", auth_code});

  HTTP_Connection->jsonReceivedSignal.connect(
      std::bind(&jsonReceived, std::placeholders::_1));
  HTTP_Connection->setURL("https://oauth2.googleapis.com/token");
  HTTP_Connection->setFields(tokenFieldsMap);
  HTTP_Connection->setMethod("POST");
  rc = HTTP_Connection->exec();
  if (rc < 0) {
    std::cout << "Could not access webpage by HTTP\n";
    return 0;
  }

  return 0;
}

int jsonReceived(std::shared_ptr<rapidjson::Document> jsonDoc) {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  jsonDoc->Accept(writer);
  std::cout << "jsonReceived:\n" << buffer.GetString() << std::endl;

  /* parse the response */
  std::string accessToken, refreshToken;
  if (!jsonDoc->IsObject()) {
    std::cout << "jsonReceived ERROR:\nJSON document invalid" << std::endl;
    return -1;
  }
  char memberName01[] = "access_token";
  if (jsonDoc->HasMember(memberName01) && (*jsonDoc)[memberName01].IsString()) {
    accessToken = (*jsonDoc)[memberName01].GetString();
  }
  char memberName02[] = "refresh_token";
  if (jsonDoc->HasMember(memberName02) && (*jsonDoc)[memberName02].IsString()) {
    refreshToken = (*jsonDoc)[memberName02].GetString();
  }

  /* Get the user's ID */
  std::string userId;
  std::cout << "Please enter your email address.\nuserId:";
  std::getline(std::cin, userId);
  std::cout << "\nuserId: " << userId << "\n";

  /* Let's test a call to the Google API!
   */
  int rc;
  std::string baseUrl = "";
  baseUrl.append("https://gmail.googleapis.com/gmail/v1/users/")
      .append(userId)
      .append("/messages");
  std::shared_ptr<bookfiler::HTTP::Connection> HTTP_Connection =
      mySQL_Module->newConnection();
  HTTP_Connection->setURL(baseUrl);
  std::shared_ptr<std::unordered_map<std::string, std::string>> fieldsMap =
      std::make_shared<std::unordered_map<std::string, std::string>>();
  fieldsMap->insert({"maxResults", "20"});
  HTTP_Connection->setFields(fieldsMap);

  std::shared_ptr<std::unordered_map<std::string, std::string>> headerMap =
      std::make_shared<std::unordered_map<std::string, std::string>>();
  headerMap->insert({"Authorization: Bearer ", accessToken});
  headerMap->insert({"Accept: ", "application/json"});
  HTTP_Connection->setHeaders(headerMap);

  HTTP_Connection->setMethod("GET");
  HTTP_Connection->jsonReceivedSignal.connect(
      std::bind(&apiJsonReceived, std::placeholders::_1));
  rc = HTTP_Connection->exec();
  if (rc < 0) {
    std::cout << "Could not access webpage by HTTP\n";
    return 0;
  }

  return 0;
}

int apiJsonReceived(std::shared_ptr<rapidjson::Document> jsonDoc) {
  /* Print the response */
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  jsonDoc->Accept(writer);
  std::cout << "apiJsonReceived:\n" << buffer.GetString() << std::endl;

  return 0;
}
