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

UrlImpl::UrlImpl(std::unordered_map<std::string, newUrlVariantType> map) {
  for (auto val : map) {
    if (int *val_ = std::get_if<int>(&val.second)) {
    } else if (double *val_ = std::get_if<double>(&val.second)) {
    } else if (std::string *val_ = std::get_if<std::string>(&val.second)) {
      if (val.first == "host") {
        set_encoded_host(*val_);
      } else if (val.first == "path") {
        set_encoded_path(*val_);
      } else if (val.first == "scheme") {
        set_scheme(*val_);
      } else if (val.first == "query") {
        set_query(*val_);
      }
    }
  }
}

UrlImpl::~UrlImpl() {}

int UrlImpl::setQuery(std::shared_ptr<queryMap> fieldsMapPtr_) {
  fieldsMapPtr = fieldsMapPtr_;
  set_query(getQueryStr());
  return 0;
}
int UrlImpl::setQuery(std::shared_ptr<rapidjson::Document> fieldsJsonDocPtr_) {
  return 0;
}
std::string UrlImpl::getQueryStr() {
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

std::string_view UrlImpl::getURL() {
  return std::string_view(encoded_url().data(), encoded_url().size());
}

} // namespace HTTP
} // namespace bookfiler
