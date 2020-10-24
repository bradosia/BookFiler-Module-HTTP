/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Bookfiler Modules
#include <BookFilerModuleHttpLoader.hpp>

std::string routeAll(bookfiler::HTTP::request req,
                     bookfiler::HTTP::response res);
int allModulesLoaded();
int jsonReceived(std::shared_ptr<rapidjson::Document>);
int apiJsonReceived(std::shared_ptr<rapidjson::Document>);

std::string testName = "Oauth2 Example";
std::shared_ptr<bookfiler::HTTP::ModuleInterface> httpModule;
std::shared_ptr<bookfiler::HTTP::Server> httpServer;

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

  // SSL Certificate manager
  std::shared_ptr<bookfiler::certificate::Manager> certificateManager =
      httpModule->newCertificateManager();
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
      httpModule->newServer({});
  httpServer->useCertificate(certRootPtr);
  httpServer->runAsync();
  httpServer->route({{"method", "GET"},
                     {"path", "/"},
                     {"handler", std::bind(&routeAll, std::placeholders::_1,
                                           std::placeholders::_2)}});

  // authorization url
  std::shared_ptr<bookfiler::HTTP::Url> authUrl = httpModule->newUrl();
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
      httpModule->newConnection();

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
      httpModule->newConnection();
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

std::string routeAll(bookfiler::HTTP::request req,
                     bookfiler::HTTP::response res) {
  std::string bodyStr = "<h1>URL Data</h1><br>";
  bodyStr.append(req->getRequest()->target().data());
  return bodyStr;
}
