/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Bookfiler Modules
#include <BookFilerModuleHttpLoader.hpp>

std::string routeAbout(bookfiler::HTTP::request req,
                       bookfiler::HTTP::response res);
int allModulesLoaded();

std::string testName = "HTTP Server Example";
std::shared_ptr<bookfiler::HTTP::ModuleInterface> httpModule;
std::shared_ptr<bookfiler::HTTP::Server> httpServer;

int main() {
  std::cout << testName << " BEGIN" << std::endl;

  bookfiler::HTTP::loadModule("modules", std::bind(&allModulesLoaded),
                              httpModule);

  std::cout << testName << " END" << std::endl;

  system("pause");
  return 0;
}

int allModulesLoaded() {
  // SSL Certificate manager
  std::shared_ptr<bookfiler::certificate::Manager> certificateManager =
      httpModule->newCertificateManager();
  std::shared_ptr<bookfiler::certificate::Certificate> certRootPtr,
      certServerPtr;
  certificateManager->loadCertificate(certRootPtr);

  // Setup server
  httpServer = httpModule->newServer({{"port", 3000}, {"host", "localhost"}});
  httpServer->useCertificate(certRootPtr);

  std::cout << "httpServer->route" << std::endl;
  // Route by using a lambda expression
  httpServer->route({{"method", "GET"},
                     {"path", "/"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                        return "Hello World!";
                      }}});

  // Route by binding a callback function
  httpServer->route({{"method", "GET"},
                     {"path", "/about"},
                     {"handler", std::bind(&routeAbout, std::placeholders::_1,
                                           std::placeholders::_2)}});

  // Start server
  httpServer->run();

  return 0;
}

std::string routeAbout(bookfiler::HTTP::request req,
                       bookfiler::HTTP::response res) {
  std::string bodyStr = "<h1>About</h1><br><h1>URL Data</h1><br>";
  bodyStr.append(req->getEncodedQuery());
  auto codeQuery = req->getQuery("code");
  if (codeQuery) {
    bodyStr.append("<br>").append(codeQuery.value());
  }
  return bodyStr;
}
