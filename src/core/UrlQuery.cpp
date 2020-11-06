/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "UrlQuery.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

QueryImpl::QueryImpl() {}

QueryImpl::QueryImpl(std::string queryStr_) {}

QueryImpl::~QueryImpl() {}

int QueryImpl::setQuery(
    std::map<std::string, std::string> queryMap_) {
  queryMap = queryMap_;
  // outputs an encoded query string
  queryEncodedStr = flattenQueryMap();
  return 0;
}
int QueryImpl::setQuery(
    std::shared_ptr<rapidjson::Document> fieldsJsonDocPtr_) {
  return 0;
}
int QueryImpl::setEncodedQuery(std::string queryStr_) {
  // query string is stored encoded, then decoded as needed
  queryEncodedStr = queryStr_;
  generateQueryMap(queryStr_);
  return 0;
}
std::string QueryImpl::flattenQueryMap() {
  std::string fieldsStr = "";
  if (!queryMap.empty()) {
    for (auto fieldIt = queryMap.begin(); fieldIt != queryMap.end();) {
      std::string key, val;
      const std::string reservedCharStr = "&=";
      util.uriEncode(fieldIt->first, reservedCharStr, key);
      util.uriEncode(fieldIt->second, reservedCharStr, val);
      fieldsStr.append(key).append("=").append(val);
      if (++fieldIt != queryMap.end()) {
        fieldsStr.append("&");
      }
    }
  }
  return fieldsStr;
}

int QueryImpl::generateQueryMap(std::string queryEncodedStr_) {
  /* parse query into map
   * Query Map key/value pairs are stored decoded
   */
  std::vector<std::string> result;
  boost::algorithm::split(result, queryEncodedStr_, boost::is_any_of("&"));
  for (auto queryPart : result) {
    const auto queryPartPos = queryPart.find_first_of('=');
    std::string key, val;
    util.uriDecode(queryPart.substr(0, queryPartPos), key);
    util.uriDecode(queryPart.substr(queryPartPos + 1), val);
    // move temporary strings into the map
    queryMap.insert({std::move(key), std::move(val)});
  }
  return 0;
}
std::string QueryImpl::getQuery() {
  std::string queryDecodedStr;
  util.uriDecode(queryEncodedStr, queryDecodedStr);
  return queryDecodedStr;
}
std::string QueryImpl::getEncodedQuery() { return queryEncodedStr; }
std::optional<std::string> QueryImpl::getQuery(std::string key_) {
  auto keyFound = queryMap.find(key_);
  if (keyFound != queryMap.end()) {
    return keyFound->second;
  }
  return {};
}

} // namespace HTTP
} // namespace bookfiler
