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

RequestImpl::RequestImpl(){
    urlPtr = std::make_shared<UrlImpl>();
}
RequestImpl::~RequestImpl(){

}

requestBeastInternal RequestImpl::getRequest() { return reqBeast; }
int RequestImpl::setRequest(requestBeastInternal reqBeast_) {
  reqBeast = reqBeast_;
  return 0;
}

std::string_view RequestImpl::url() { return targetStr; }
std::string_view RequestImpl::method() { return methodStr; }
std::string_view RequestImpl::host() { return hostStr; }
std::string RequestImpl::path() { return pathStr; }
std::string RequestImpl::query() { return queryStr; }

} // namespace HTTP
} // namespace bookfiler
