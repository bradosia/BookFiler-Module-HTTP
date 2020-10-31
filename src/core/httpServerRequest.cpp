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

requestBeastInternal RequestImpl::getRequest() { return reqBeast; }
int RequestImpl::setRequest(requestBeastInternal reqBeast_) {
  reqBeast = reqBeast_;
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
std::string RequestImpl::getEncodedQuery() { return urlPtr->getEncodedQuery(); }
std::optional<std::string> RequestImpl::getQuery(std::string key_) {
  return urlPtr->getQuery(key_);
}

} // namespace HTTP
} // namespace bookfiler
