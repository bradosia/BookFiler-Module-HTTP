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

std::shared_ptr<Request> SessionImpl::parseRequest(requestBeastInternal reqBeast) {
  std::shared_ptr<RequestImpl> reqImpl = std::make_shared<RequestImpl>();
  reqImpl->methodStr = std::string_view(reqBeast->method_string().data(),
                                        reqBeast->method_string().size());
  reqImpl->targetStr =
      std::string_view(reqBeast->target().data(), reqBeast->target().size());
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
    reqImpl->hostStr =
        std::string_view(hostIt->value().data(), hostIt->value().size());
  }
  std::vector<std::string> result;
  boost::split(result, reqImpl->targetStr, boost::is_any_of("?"));
  reqImpl->pathStr = result.at(0);
  reqImpl->queryStr = "";
  if (result.size() > 1) {
    reqImpl->queryStr = result.at(1);
  }
  reqImpl->setRequest(reqBeast);
  return std::dynamic_pointer_cast<Request>(reqImpl);
}

int SessionImpl::routeValidate(std::shared_ptr<Request> req, std::string_view method,
                  std::string_view path) {
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
