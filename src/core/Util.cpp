/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "Util.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

Util::Util() {}
Util::~Util() {}

int Util::uriEncode(const std::string &str, const std::string &reserved,
                    std::string &encodedStr) {
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
    char c = *it;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
        c == '~') {
      encodedStr += c;
    } else if (c <= 0x20 || c >= 0x7F ||
               illegalCharStr.find(c) != std::string::npos ||
               reserved.find(c) != std::string::npos) {
      std::ostringstream escaped;
      escaped.fill('0');
      // must be uppercase or boost::url will throw
      escaped << '%' << std::hex << std::uppercase << std::setw(2)
              << int((unsigned char)c);
      encodedStr += escaped.str();
    } else
      encodedStr += c;
  }
  return 0;
}

int Util::uriDecode(const std::string &str, std::string &decodedStr) {
  std::string::const_iterator it = str.begin();
  std::string::const_iterator end = str.end();
  while (it != end) {
    char c = *it++;
    if (c == '%') {
      if (it == end) {
        std::cout << "URI encoding: no hex digit following percent sign"
                  << std::endl;
        return -1;
      }
      char hi = *it++;
      if (it == end) {
        std::cout << "URI encoding: two hex digits must follow percent sign"
                  << std::endl;
        return -1;
      }
      char lo = *it++;
      if (hi >= '0' && hi <= '9') {
        c = hi - '0';
      } else if (hi >= 'A' && hi <= 'F') {
        c = hi - 'A' + 10;
      } else if (hi >= 'a' && hi <= 'f') {
        c = hi - 'a' + 10;
      } else {
        std::cout << "URI encoding: not a hex digit" << std::endl;
        return -1;
      }
      c *= 16;
      if (lo >= '0' && lo <= '9') {
        c += lo - '0';
      } else if (lo >= 'A' && lo <= 'F') {
        c += lo - 'A' + 10;
      } else if (lo >= 'a' && lo <= 'f') {
        c += lo - 'a' + 10;
      } else {
        std::cout << "URI encoding: not a hex digit" << std::endl;
        return -1;
      }
    }
    decodedStr += c;
  }
  return 0;
}

void logStatus(std::string functionStr, std::string msg) {
  ::bookfiler::logStatus(moduleCode, functionStr, msg);
}

void logStatus(std::string functionStr, std::string msg,
               boost::system::error_code ec) {
  ::bookfiler::logStatus(moduleCode, functionStr, msg, ec);
}

void logConnectionStatus(unsigned int socketId, std::string functionStr,
                         std::string msg) {
  ::bookfiler::logConnectionStatus(moduleCode, socketId, functionStr, msg);
}

void logConnectionStatus(unsigned int socketId, std::string functionStr,
                         std::string msg, boost::system::error_code ec) {
  ::bookfiler::logConnectionStatus(moduleCode, socketId, functionStr, msg, ec);
}

std::string readFile(std::string path) { return ::bookfiler::readFile(path); }

} // namespace HTTP
} // namespace bookfiler
