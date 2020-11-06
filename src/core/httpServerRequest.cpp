/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServerRequest.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

RequestImpl::RequestImpl() { urlPtr = std::make_shared<UrlImpl>(); }
RequestImpl::~RequestImpl() {}

requestBeastInternal RequestImpl::getRequest() { return requestBeast; }
int RequestImpl::setRequest(requestBeastInternal requestBeast_) {
  requestBeast = requestBeast_;
  return 0;
}

std::string_view RequestImpl::url() { return urlPtr->url(); }
std::string_view RequestImpl::method() { return methodStr; }
std::string_view RequestImpl::host() {
  return std::string_view(urlPtr->encoded_host().data(),
                          urlPtr->encoded_host().size());
}
std::string RequestImpl::path() {
  return std::string(urlPtr->encoded_path().data(),
                     urlPtr->encoded_path().size());
}

int RequestImpl::setHeader(
    std::unordered_map<std::string, std::string> headerMap_) {
  for (auto entry : headerMap_) {
    requestBeast->set(entry.first, entry.second);
  }
  return 0;
}

std::string RequestImpl::getEncodedQuery() { return urlPtr->getEncodedQuery(); }
std::optional<std::string> RequestImpl::getQuery(std::string key_) {
  return urlPtr->getQuery(key_);
}

int RequestImpl::parseRequest() {
  if (!requestBeast) {
    return -1;
  }
  methodStr = std::string_view(requestBeast->method_string().data(),
                               requestBeast->method_string().size());
  const auto pos = requestBeast->target().find_first_of('?');
  if (std::string::npos != pos) {
    urlPtr->set_encoded_path(requestBeast->target().substr(0, pos));
    urlPtr->setEncodedQuery(
        std::string(requestBeast->target().substr(pos + 1).data(),
                    requestBeast->target().substr(pos + 1).size()));
  } else {
    urlPtr->set_encoded_path(requestBeast->target());
  }
  auto userAgentIt = requestBeast->base().find("User-Agent");
  if (userAgentIt != requestBeast->base().end()) {
    userAgentStr = std::string_view(userAgentIt->value().data(),
                                    userAgentIt->value().size());
  }
  auto refererIt = requestBeast->base().find("Referer");
  if (refererIt != requestBeast->base().end()) {
    refererStr =
        std::string_view(refererIt->value().data(), refererIt->value().size());
  }
  auto hostIt = requestBeast->base().find("Host");
  if (hostIt != requestBeast->base().end()) {
    urlPtr->set_host(hostIt->value());
  }
  setRequest(requestBeast);
  return 0;
}

int RequestImpl::routeValidate(std::string_view method_,
                               std::string_view path_) {
  if (method() != method_) {
    return -1;
  }
  if (path() != path_) {
    return -1;
  }
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
