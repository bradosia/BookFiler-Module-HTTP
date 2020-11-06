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

ResponseImpl::ResponseImpl() {
  writableEndedFlag = false;
  bodyStrPtr = std::make_shared<std::string>();
}

ResponseImpl::~ResponseImpl() {}

int ResponseImpl::end(std::string bodyStr) {
  *bodyStrPtr = bodyStr;
  writableEndedFlag = true;
  return 0;
}

void ResponseImpl::nothing() { return; }

bool ResponseImpl::writableEnded() { return writableEndedFlag; }

std::shared_ptr<std::string> ResponseImpl::body() { return bodyStrPtr; }

int ResponseImpl::parseResponse() { return 0; }

} // namespace HTTP
} // namespace bookfiler
