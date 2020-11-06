/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/exception/all.hpp>

// Local Project
#include "Url.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

UrlImpl::UrlImpl() {}

UrlImpl::UrlImpl(std::map<std::string, newUrlVariantType> map) {
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

int UrlImpl::setQuery(std::map<std::string, std::string> queryMap_) {
  QueryImpl::setQuery(queryMap_);
  try {
    set_encoded_query(QueryImpl::getEncodedQuery());
  } catch (std::exception &e) {
    std::cout << "UrlImpl::setQuery EXCEPTION: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
int UrlImpl::setQuery(std::shared_ptr<rapidjson::Document> fieldsJsonDocPtr_) {
  QueryImpl::setQuery(fieldsJsonDocPtr_);
  try {
    set_encoded_query(QueryImpl::getEncodedQuery());
  } catch (std::exception &e) {
    std::cout << "UrlImpl::setQuery EXCEPTION: " << e.what() << std::endl;
    return -1;
  }
  return 0;
}
int UrlImpl::setEncodedQuery(std::string queryStr_) {
  try {
    QueryImpl::setEncodedQuery(queryStr_);
  } catch (std::exception &e) {
    std::cout << "UrlImpl::setEncodedQuery EXCEPTION: " << e.what()
              << std::endl;
    return -1;
  }
  return 0;
}

std::string UrlImpl::getEncodedQuery() { return QueryImpl::getEncodedQuery(); }
std::optional<std::string> UrlImpl::getQuery(std::string key_) {
  return QueryImpl::getQuery(key_);
}

std::string_view UrlImpl::url() {
  return std::string_view(encoded_url().data(), encoded_url().size());
}

std::string_view UrlImpl::getEncodedHost() {
  return std::string_view(encoded_host().data(), encoded_host().size());
}

std::string UrlImpl::target() {
  std::string targetStr;
  targetStr.append(std::string(encoded_path().data(), encoded_path().size()))
      .append("?")
      .append(getEncodedQuery());
  return targetStr;
}

} // namespace HTTP
} // namespace bookfiler
