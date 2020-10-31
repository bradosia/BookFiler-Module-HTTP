/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// C++
#include <condition_variable>

// Bookfiler Modules
#define BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE 1
#include <BookFilerModuleHttpLoader.hpp>

std::mutex globalMutex;
bool authFlag, userIdFlag, applicationFlag;
std::condition_variable authCondition, userIdCondition, applicationCondition;
std::string authCode, userId, accessToken, refreshToken;

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
  authFlag = userIdFlag = false;

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
  std::shared_ptr<bookfiler::HTTP::Url> authUrl =
      httpModule->newUrl({{"host", "accounts.google.com"},
                          {"path", "/o/oauth2/v2/auth"},
                          {"scheme", "https"}});
  authUrl->setQuery(
      {{"client_id", "854776203850-r64s69l8jmh71ugiio16impqfcp80j1m.apps."
                     "googleusercontent.com"},
       {"scope", "https://mail.google.com/"},
       {"response_type", "code"},
       {"redirect_uri", "https://localhost:8081"},
       {"test", "/*&op=4$@"}});

  std::cout << "allModulesLoaded::authUrl->getURL() = " << authUrl->url()
            << "\n\n";

  // open google oauth2 window
  std::string windowOpenCommand = "explorer \"";
  windowOpenCommand.append(authUrl->url()).append("\"");
  system(windowOpenCommand.c_str());

  std::cout << "\n===MAIN THREAD===\nPlease login to your google account in "
               "the new window.";
  {
    std::unique_lock<std::mutex> mutexLock(globalMutex);
    authCondition.wait(mutexLock, [] { return authFlag; });
  }
  std::cout << "\n===MAIN THREAD===\nAuthorization Code: " << authCode << "\n";

  /* Start an HTTP post request to get the access token
   */
  std::shared_ptr<bookfiler::HTTP::Client> httpClient = httpModule->newClient({
      {"host", "oauth2.googleapis.com"},
      {"path", "/token"},
      {"scheme", "https"},
      {"method", "POST"},
  });

  std::cout << "allModulesLoaded httpClient->url() = " << httpClient->url()
            << "\n\n";

  // token url
  httpClient->jsonReceivedSignal.connect(
      std::bind(&jsonReceived, std::placeholders::_1));
  httpClient->setQuery(
      {{"client_id", "854776203850-r64s69l8jmh71ugiio16impqfcp80j1m.apps."
                     "googleusercontent.com"},
       {"client_secret", "18e_rRNMBIJOJ0jWUthY7RKp"},
       {"grant_type", "authorization_code"},
       {"redirect_uri", "https://localhost:8081"},
       {"code", authCode}});

  std::cout << "allModulesLoaded httpClient->url() = " << httpClient->url()
            << "\n\n";

  rc = httpClient->end();

  std::cout << "allModulesLoaded httpClient->url() = " << httpClient->url()
            << "\n\n";
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
  std::cout << "\n===MAIN THREAD===\nPlease enter your google user ID "
               "into the form on the new window.";
  {
    std::unique_lock<std::mutex> mutexLock(globalMutex);
    userIdCondition.wait(mutexLock, [] { return userIdFlag; });
  }
  std::cout << "\n===MAIN THREAD===\nUser ID: " << userId << "\n";

  std::cout << "\n===MAIN THREAD===\nApplication waiting until shut down.";
  {
    std::unique_lock<std::mutex> mutexLock(globalMutex);
    userIdCondition.wait(mutexLock, [] { return applicationFlag; });
  }
  std::cout << "\n===MAIN THREAD===\nApplication Shutting Done\n";

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
  std::string bodyStr = "<h1>Google OAUTH2 Example</h1>";
  auto codeQuery = req->getQuery("code");
  auto userIdQuery = req->getQuery("userId");
  auto messagesPageQuery = req->getQuery("messagesPage");
  if (userIdQuery) {
    userId = userIdQuery.value();
    userIdFlag = true;
    userIdCondition.notify_one();
    bodyStr.append("<br>userId=")
        .append(userId)
        .append("<form action=\"/\" method=\"GET\"><label for=\"resultNum"
                "\">Results:</label><br><input "
                "type=\"text\" id=\"resultNum"
                "\" name=\"resultNum\" value=\"20\"><br><input type=\"hidden\" "
                "name=\"messagesPage\" value=\"1\"><input type=\"hidden\" "
                "name=\"userId\" value=\"")
        .append(userId)
        .append("\"><input type=\"submit"
                "\" value=\"Submit\"></form>");
    if (messagesPageQuery) {
      std::string resultsNum = "20";
      auto resultNumQuery = req->getQuery("resultNum");
      if (resultNumQuery) {
        resultsNum = resultNumQuery.value();
      }
      /* Let's test a call to the Google API!
       */
      int rc;
      std::string baseUrl = "";
      baseUrl.append("https://gmail.googleapis.com/gmail/v1/users/")
          .append(userId)
          .append("/messages");
      std::shared_ptr<bookfiler::HTTP::Client> httpClient =
          httpModule->newClient();
      httpClient->setURL(baseUrl);
      httpClient->setQuery({{"maxResults", resultsNum}});

      std::shared_ptr<std::unordered_map<std::string, std::string>> headerMap =
          std::make_shared<std::unordered_map<std::string, std::string>>();
      headerMap->insert({"Authorization: Bearer ", accessToken});
      headerMap->insert({"Accept: ", "application/json"});
      httpClient->setHeaders(headerMap);

      httpClient->setMethod("GET");
      httpClient->jsonReceivedSignal.connect(
          std::bind(&apiJsonReceived, std::placeholders::_1));
      rc = httpClient->end();
      if (rc < 0) {
        bodyStr.append("Could not access webpage by HTTP\n");
        std::cout << "Could not access webpage by HTTP\n";
        return bodyStr;
      }
    }
  } else if (codeQuery) {
    authCode = codeQuery.value();
    authFlag = true;
    authCondition.notify_one();
    bodyStr.append("<br>code=")
        .append(authCode)
        .append("<form action=\"/\" method=\"GET\"><label for=\"userId"
                "\">Google User ID (Email address):</label><br><input "
                "type=\"text\" id=\"userId"
                "\" name=\"userId\" value=\"\"><br><input type=\"submit"
                "\" value=\"Submit\"></form>");
  } else {
    bodyStr.append("<br>Error: Code not received");
  }
  bodyStr.append("<br>path: ").append(req->path());
  return bodyStr;
}
