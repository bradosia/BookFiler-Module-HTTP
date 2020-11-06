/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "RequestUserAgent.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

UserAgentImpl::UserAgentImpl() {
  uap_cpp::UserAgentParser userAgentParser("resources/regexes.yaml");
  userAgent = userAgentParser.parse(
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
      "like Gecko) Chrome/86.0.4240.183 Safari/537.36");
}

UserAgentImpl::~UserAgentImpl() {}

std::string UserAgentImpl::getBrowser() { return userAgent.browser.family; }

} // namespace HTTP
} // namespace bookfiler
