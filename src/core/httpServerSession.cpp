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

std::shared_ptr<Request>
SessionImpl::parseRequest(requestBeastInternal reqBeast) {
  std::shared_ptr<RequestImpl> reqImpl = std::make_shared<RequestImpl>();
  reqImpl->methodStr = std::string_view(reqBeast->method_string().data(),
                                        reqBeast->method_string().size());
  const auto pos = reqBeast->target().find_first_of('?');
  if (std::string::npos != pos) {
    reqImpl->urlPtr->set_encoded_path(reqBeast->target().substr(0, pos));
    reqImpl->urlPtr->setEncodedQuery(
        std::string(reqBeast->target().substr(pos + 1).data(),
                    reqBeast->target().substr(pos + 1).size()));
  } else {
    reqImpl->urlPtr->set_encoded_path(reqBeast->target());
  }
  auto userAgentIt = reqBeast->base().find("User-Agent");
  if (userAgentIt != reqBeast->base().end()) {
    reqImpl->userAgentStr = std::string_view(userAgentIt->value().data(),
                                             userAgentIt->value().size());
  }
  auto refererIt = reqBeast->base().find("Referer");
  if (refererIt != reqBeast->base().end()) {
    reqImpl->refererStr =
        std::string_view(refererIt->value().data(), refererIt->value().size());
  }
  auto hostIt = reqBeast->base().find("Host");
  if (hostIt != reqBeast->base().end()) {
    reqImpl->urlPtr->set_host(hostIt->value());
  }
  reqImpl->setRequest(reqBeast);
  return std::dynamic_pointer_cast<Request>(reqImpl);
}

int SessionImpl::routeValidate(std::shared_ptr<Request> req,
                               std::string_view method, std::string_view path) {
  std::shared_ptr<RequestImpl> reqImpl =
      std::static_pointer_cast<RequestImpl>(req);
  if (reqImpl->method() != method) {
    return -1;
  }
  if (reqImpl->path() != path) {
    return -1;
  }
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
