/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpHeader.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

HeaderImpl::HeaderImpl() {}

HeaderImpl::HeaderImpl(std::string headerStr_) { setHeaderStr(headerStr_); }

int HeaderImpl::setHeader(std::unordered_map<std::string, std::string>){
return 0;
}

HeaderImpl::~HeaderImpl() {}

} // namespace HTTP
} // namespace bookfiler
