/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// C++17
#include <filesystem>

/* bustache 0.1.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <bustache/model.hpp>
#include <bustache/render/string.hpp>

#include <nlohmann/json.hpp>

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

// Typically, you don't need to explicitly delete the impl_model, but
// nlohmann::json is basically a amoeba that prentends to be any model, and that
// will cause hard errors in concept checking due to recursion.
template <> struct bustache::impl_model<nlohmann::json> {
  impl_model() = delete;
};

template <> struct bustache::impl_compatible<nlohmann::json> {
  static value_ptr get_value_ptr(nlohmann::json const &self) {
    nlohmann::json::value_t const kind(self);
    switch (kind) {
    case nlohmann::json::value_t::boolean:
      return value_ptr(self.get_ptr<nlohmann::json::boolean_t const *>());
    case nlohmann::json::value_t::number_integer:
      return value_ptr(
          self.get_ptr<nlohmann::json::number_integer_t const *>());
    case nlohmann::json::value_t::number_unsigned:
      return value_ptr(
          self.get_ptr<nlohmann::json::number_unsigned_t const *>());
    case nlohmann::json::value_t::number_float:
      return value_ptr(self.get_ptr<nlohmann::json::number_float_t const *>());
    case nlohmann::json::value_t::string:
      return value_ptr(self.get_ptr<nlohmann::json::string_t const *>());
    case nlohmann::json::value_t::array:
      return value_ptr(self.get_ptr<nlohmann::json::array_t const *>());
    case nlohmann::json::value_t::object:
      return value_ptr(self.get_ptr<nlohmann::json::object_t const *>());
    }
    return value_ptr();
  }
};

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

struct file_context {
  struct partail {
    std::string text;
    bustache::format format;

    void init(std::string const &filename) {
      text = read_file(filename.c_str());
      format = bustache::format(text);
    }
  };
  mutable std::unordered_map<std::string, partail> cache;

  bustache::format const *operator()(std::string const &key) const {
    auto [pos, inserted] = cache.try_emplace(key);
    if (inserted)
      pos->second.init(key + ".mustache");
    return pos->second.text.empty() ? nullptr : &pos->second.format;
  }
};

std::string routeHome(bookfiler::HTTP::request req,
                      bookfiler::HTTP::response res) {
  std::string bodyStr = "";
  try {
    std::string jsonStr = read_file("resources/templates/in.json");
    auto const json = nlohmann::json::parse(jsonStr);
    std::string file = read_file("resources/www/index.html");
    std::cout << file << std::endl;
    bustache::format fmt(file);
    file_context ctx;
    bodyStr = bustache::to_string(fmt(json).context(ctx));
  } catch (const std::exception &e) {
    std::cerr << e.what();
  }

  return bodyStr;
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
  std::string bodyStr = "";
  try {
    std::string jsonStr = read_file("resources/templates/in.json");
    auto const json = nlohmann::json::parse(jsonStr);
    std::string file = read_file("resources/www/static/nicepage.css");
    std::cout << file << std::endl;
    bustache::format fmt(file);
    file_context ctx;
    bodyStr = bustache::to_string(fmt(json).context(ctx));
  } catch (const std::exception &e) {
    std::cerr << e.what();
  }

  return bodyStr;
}

std::string route404(bookfiler::HTTP::request req,
                     bookfiler::HTTP::response res) {
  std::string bodyStr = "";
  try {
    std::string jsonStr = read_file("resources/templates/in.json");
    auto const json = nlohmann::json::parse(jsonStr);
    std::string file = read_file("resources/www/index.html");
    std::cout << file << std::endl;
    bustache::format fmt(file);
    file_context ctx;
    bodyStr = bustache::to_string(fmt(json).context(ctx));
  } catch (const std::exception &e) {
    std::cerr << e.what();
  }

  return bodyStr;
}
