/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_CONFIG_H
#define BOOKFILER_MODULE_HTTP_CONFIG_H

#define HTTPS_GET_JSON_DEBUG 0
#define HTTPS_GET_JSON_ENABLE 1
#define HTTPS_CURL_ENABLE 1
#define RSA_KEY_LENGTH 2048

#include <string>

namespace bookfiler {
namespace HTTP {

static std::string moduleName = "BookFiler Module HTTP";
static std::string moduleCode = "bookfiler::HTTP";

} // namespace MySQL
} // namespace bookfiler

#endif // BOOKFILER_MODULE_MYSQL_CONFIG_H
