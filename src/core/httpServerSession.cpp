/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServerSession.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

int SessionImpl::routeValidate(std::shared_ptr<Request> req, std::string_view method,
                  std::string_view path) {
  std::shared_ptr<RequestImpl> reqImpl =
      std::static_pointer_cast<RequestImpl>(req);
  return reqImpl->routeValidate(method, path);
}
std::shared_ptr<Request> SessionImpl::parseRequest(requestBeast reqBeast) {
  std::shared_ptr<RequestImpl> reqImpl = std::make_shared<RequestImpl>();
  reqImpl->setRequest(reqBeast);
  reqImpl->parseRequest();
  return std::dynamic_pointer_cast<Request>(reqImpl);
}

} // namespace HTTP
} // namespace bookfiler
