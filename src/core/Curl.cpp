/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "Curl.hpp"

/*
 * bookfiler - Curl
 */
namespace bookfiler {
namespace curl {

InitImpl::InitImpl() { curl_global_init(CURL_GLOBAL_DEFAULT); }

InitImpl::~InitImpl() { curl_global_cleanup(); }

size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s) {
  s->append((char *)ptr, size * nmemb);
  return size * nmemb;
}

} // namespace curl
} // namespace bookfiler
