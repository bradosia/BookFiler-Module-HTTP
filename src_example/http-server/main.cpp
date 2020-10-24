/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Bookfiler Modules
#include <BookFilerModuleHttpLoader.hpp>

int routeAll(bookfiler::HTTP::settings settings, bookfiler::HTTP::requestBeast req,
             bookfiler::HTTP::responseBeast res);
std::string routeAbout(bookfiler::HTTP::request req,
                       bookfiler::HTTP::response res);
int allModulesLoaded();

std::string testName = "HTTP Server Example";
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
  // SSL Certificate manager
  std::shared_ptr<bookfiler::certificate::Manager> certificateManager =
      httpModule->newCertificateManager();
  std::shared_ptr<bookfiler::certificate::Certificate> certRootPtr,
      certServerPtr;
  certificateManager->loadCertificate(certRootPtr);

  // Setup server
  httpServer = httpModule->newServer({{"port", 3000}, {"host", "localhost"}});
  httpServer->useCertificate(certRootPtr);

  // Route using signals and slots
  httpServer->getRouteSignal()->connect(&routeAll);

  // Route by using a lambda expression
  httpServer->route({{"method", "GET"},
                     {"path", "/"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                        return "Hello World!";
                      }}});

  // Route by binding a callback function
  httpServer->route(
      {{"method", "GET"},
       {"path", "/about"},
       {"handler", std::bind(&routeAbout, std::placeholders::_1,
                             std::placeholders::_2)}});

  // Start server
  httpServer->runAsync();

  return 0;
}

int routeAll(bookfiler::HTTP::settings settings,
             bookfiler::HTTP::requestBeast req,
             bookfiler::HTTP::responseBeast res) {
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

std::string routeAbout(bookfiler::HTTP::request req,
                       bookfiler::HTTP::response res) {
  std::string bodyStr = "<h1>About</h1><br><h1>URL Data</h1><br>";
  bodyStr.append(req->getRequest()->target().data());
  std::cout << bodyStr << std::endl;
  return bodyStr;
}
