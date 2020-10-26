/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServerResponse.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

responseBeastInternal ResponseImpl::getResponse() { return resBeast; }
int ResponseImpl::setResponse(responseBeastInternal resBeast_) {
  resBeast = resBeast_;
  return 0;
}
int ResponseImpl::send(std::string) { return 0; }

} // namespace HTTP
} // namespace bookfiler
