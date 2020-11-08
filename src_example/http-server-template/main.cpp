/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// C++17
#include <filesystem>

// Bookfiler Modules
#include <BookFilerModuleHttpLoader.hpp>

std::string routeHome(bookfiler::HTTP::request req,
                      bookfiler::HTTP::response res);
std::string routeAbout(bookfiler::HTTP::request req,
                       bookfiler::HTTP::response res);
std::string routeCSS(bookfiler::HTTP::request req,
                     bookfiler::HTTP::response res);
std::string route404(bookfiler::HTTP::request req,
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
  httpServer->route({{"method", "GET"},
                     {"path", "/"},
                     {"handler", std::bind(&routeHome, std::placeholders::_1,
                                           std::placeholders::_2)}});

  httpServer->route({{"method", "GET"},
                     {"path", "/Home.html"},
                     {"handler", std::bind(&routeHome, std::placeholders::_1,
                                           std::placeholders::_2)}});

  httpServer->route({{"method", "GET"},
                     {"path", "/about"},
                     {"handler", std::bind(&routeAbout, std::placeholders::_1,
                                           std::placeholders::_2)}});

  httpServer->route({{"method", "GET"},
                     {"path", "/static/nicepage.css"},
                     {"handler", std::bind(&routeCSS, std::placeholders::_1,
                                           std::placeholders::_2)}});

  httpServer->route({{"method", "GET"},
                     {"path", "*"},
                     {"handler", std::bind(&route404, std::placeholders::_1,
                                           std::placeholders::_2)}});

  // Start server
  httpServer->run();

  return 0;
}

std::string read_file(char const *path) {
  std::string ret;
  if (auto const fd = std::fopen(path, "rb")) {
    auto const bytes = std::filesystem::file_size(path);
    ret.resize(bytes);
    std::fread(ret.data(), 1, bytes, fd);
    std::fclose(fd);
  }
  return ret;
}

std::string routeHome(bookfiler::HTTP::request req,
                      bookfiler::HTTP::response res) {
  std::optional<rapidjson::Document> jsonDocOpt =
      httpModule->Json()->readFile("resources/templates/in.json");
  if (!jsonDocOpt) {
    return "";
  }
  std::shared_ptr<bookfiler::Template> tmpl = httpModule->newTemplate();
  tmpl->readFile("resources/www/index.html");
  tmpl->mergeData(*jsonDocOpt);
  tmpl->addContext("href", "href=\"{{url}}\"");
  tmpl->parse();
  return tmpl->toString();
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

std::string routeCSS(bookfiler::HTTP::request req,
                     bookfiler::HTTP::response res) {
  std::optional<rapidjson::Document> jsonDocOpt =
      httpModule->Json()->readFile("resources/templates/in.json");
  if (!jsonDocOpt) {
    return "";
  }
  std::shared_ptr<bookfiler::Template> tmpl = httpModule->newTemplate();
  tmpl->readFile("resources/www/static/nicepage.css");
  tmpl->mergeData(*jsonDocOpt);
  tmpl->addContext("href", "href=\"{{url}}\"");
  tmpl->parse();
  return tmpl->toString();
}

std::string route404(bookfiler::HTTP::request req,
                     bookfiler::HTTP::response res) {
  std::optional<rapidjson::Document> jsonDocOpt =
      httpModule->Json()->readFile("resources/templates/in.json");
  if (!jsonDocOpt) {
    return "";
  }
  std::shared_ptr<bookfiler::Template> tmpl = httpModule->newTemplate();
  tmpl->readFile("resources/www/index.html");
  tmpl->mergeData(*jsonDocOpt);
  tmpl->addContext("href", "href=\"{{url}}\"");
  tmpl->parse();
  return tmpl->toString();
}
