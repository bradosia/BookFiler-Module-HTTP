/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "Response.hpp"

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

int ResponseImpl::parseResponse() {
    // move data from Response to the underlying boost::beast::http:message
    return 0;
}

int ResponseImpl::setCookie(std::string name_, std::string value_,
                            std::map<std::string, newCookieVariantType> map_) {
  /*
  cookieMap.addCookie(name_, value_, map_);
  */
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
