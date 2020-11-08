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
void signal_handler(int signal);

std::string testName = "HTTP Server Example";
std::shared_ptr<bookfiler::HTTP::ModuleInterface> httpModule;
std::shared_ptr<bookfiler::HTTP::Server> httpServer;

int main() {
  std::cout << testName << " BEGIN" << std::endl;

  bookfiler::HTTP::loadModule("modules", std::bind(&allModulesLoaded),
                              httpModule);

  std::cout << testName << " END" << std::endl;
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

  httpServer->route({{"method", "GET"},
                     {"path", "/get"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                        // Get cookie
                        std::string_view value =
                            req->getCookie("name")->value();
                        std::string bodyStr;
                        bodyStr.append("Get cookie name=").append(value);
                        return bodyStr;
                      }}});

  httpServer->route({{"method", "GET"},
                     {"path", "/user"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                        // Get cookie
                        std::string value = req->getUserAgentBrowser();
                        std::string bodyStr;
                        bodyStr.append("User Agent Browser=").append(value);
                        return bodyStr;
                      }}});

  httpServer->route(
      {{"method", "GET"},
       {"path", "*"},
       {"handler",
        [](bookfiler::HTTP::request req,
           bookfiler::HTTP::response res) -> std::string {
          // Set cookie
          res->setCookie("name", "value", {});
          return "<h1>Set cookie</h1><br><a href=\"/get\">Get Cookie</a>";
        }}});

  // Start server
  httpServer->run();

  std::cout << "\n===MAIN THREAD===\nApplication waiting until shut down.";
  httpModule->wait("exit");
  std::cout << "\n===MAIN THREAD===\nApplication Shutting Done\n";

  return 0;
}
