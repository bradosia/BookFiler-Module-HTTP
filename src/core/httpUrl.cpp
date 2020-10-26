/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpUrl.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

UrlImpl::UrlImpl() {}

UrlImpl::~UrlImpl() {}

int UrlImpl::setBase(std::string base_) {
  base = base_;
  return 0;
}
int UrlImpl::setFields(
    std::shared_ptr<std::unordered_map<std::string, std::string>>
        fieldsMapPtr_) {
  fieldsMapPtr = fieldsMapPtr_;
  return 0;
}
int UrlImpl::setFields(std::shared_ptr<rapidjson::Document> fieldsJsonDocPtr_) {
  return 0;
}
std::string UrlImpl::getURL() {
  std::string url = base;
  if (fieldsMapPtr && !fieldsMapPtr->empty()) {
    url.append("?").append(getFieldsStr());
  }
  return url;
}
std::string UrlImpl::getBase() {
  std::string url = base;
  return url;
}
std::string UrlImpl::getFieldsStr() {
  std::string fieldsStr = "";
  if (fieldsMapPtr && !fieldsMapPtr->empty()) {
    for (auto fieldIt = fieldsMapPtr->begin();
         fieldIt != fieldsMapPtr->end();) {
      fieldsStr.append(fieldIt->first).append("=").append(fieldIt->second);
      if (++fieldIt != fieldsMapPtr->end()) {
        fieldsStr.append("&");
      }
    }
  }
  return fieldsStr;
}

} // namespace HTTP
} // namespace bookfiler
