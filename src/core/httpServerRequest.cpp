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

requestBeast RequestImpl::getRequest() { return reqBeast; }
int RequestImpl::setRequest(requestBeast reqBeast_) {
  reqBeast = reqBeast_;
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
