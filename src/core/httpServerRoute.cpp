/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpServerRoute.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

RouteImpl::RouteImpl() {}
RouteImpl::~RouteImpl() {}
int RouteImpl::getAdd(std::string path, int priority,
                      routeFunctionTypeInternal routeFunction) {
  std::stringstream ss;
  ss << "START\npath=" << path << " priority=" << priority;
  logStatus("::RouteImpl::getAdd", ss.str());
  return add(path, priority, routeFunction, routeGetMap);
}
int RouteImpl::postAdd(std::string path, int priority,
                       routeFunctionTypeInternal routeFunction) {
  std::stringstream ss;
  ss << "START\npath=" << path << " priority=" << priority;
  logStatus("::RouteImpl::postAdd", ss.str());
  return add(path, priority, routeFunction, routePostMap);
}
int RouteImpl::allAdd(std::string path, int priority,
                      routeFunctionTypeInternal routeFunction) {
  std::stringstream ss;
  ss << "START\npath=" << path << " priority=" << priority;
  logStatus("::RouteImpl::allAdd", ss.str());
  return add(path, priority, routeFunction, routeAllMap);
}
int RouteImpl::add(
    std::string path, int priority, routeFunctionTypeInternal routeFunction,
    std::map<std::string, std::shared_ptr<routeSignalTypeInternal>> &routeMap) {
  // First see if the path already exists so the signal can be reused.
  auto it = routeMap.find(path);
  std::shared_ptr<routeSignalTypeInternal> pathSignal;
  if (it == routeMap.end()) {
    pathSignal = std::make_shared<routeSignalTypeInternal>();
    routeMap.insert({path, pathSignal});
  } else {
    pathSignal = it->second;
  }
  pathSignal->connect(priority, routeFunction);
  return 0;
}

int RouteImpl::doGetSignal(std::string path, std::shared_ptr<RequestImpl> req,
                           std::shared_ptr<ResponseImpl> res) {
  std::stringstream ss;
  ss << "START\npath=" << path;
  logStatus("::RouteImpl::doGetSignal", ss.str());
  return doSignal(path, req, res, routeGetMap);
}

int RouteImpl::doSignal(
    std::string path, std::shared_ptr<RequestImpl> reqImpl,
    std::shared_ptr<ResponseImpl> resImpl,
    std::map<std::string, std::shared_ptr<routeSignalTypeInternal>> &routeMap) {
  auto it = routeMap.find(path);
  if (it != routeMap.end()) {
    goto success;
  }
  // try the "*" path as backup
  it = routeMap.find("*");
  if (it != routeMap.end()) {
    goto success;
  }
  // try the all map
  it = routeAllMap.find(path);
  if (it != routeAllMap.end()) {
    goto success;
  }
  // try the all map
  it = routeAllMap.find("*");
  if (it != routeAllMap.end()) {
    goto success;
  }
  // could not find any match
  return -1;
success:
  std::shared_ptr<routeSignalTypeInternal> pathSignal = it->second;
  // Set combiner argument
  RouteCombiner routeCombiner;
  routeCombiner.setRequest(reqImpl);
  routeCombiner.setResponse(resImpl);
  pathSignal->set_combiner(routeCombiner);
  // Call signal
  std::shared_ptr<Request> req = std::dynamic_pointer_cast<Request>(reqImpl);
  std::shared_ptr<Response> res = std::dynamic_pointer_cast<Response>(resImpl);
  boost::optional<std::string> returnStrOpt = (*pathSignal)(req, res);
  if (!returnStrOpt) {
    return -1;
  }
  std::string bodyStr = *returnStrOpt;
  requestBeastInternal reqBeast = reqImpl->getRequest();
  responseBeastInternal resBeast = resImpl->getResponse();
  resBeast->result(boost::beast::http::status::ok);
  resBeast->version(reqBeast->version());
  resBeast->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  resBeast->set(boost::beast::http::field::content_type, "text/html");
  resBeast->keep_alive(reqBeast->keep_alive());
  resBeast->body() = bodyStr;
  resBeast->content_length(resBeast->body().length());
  resBeast->prepare_payload();
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
