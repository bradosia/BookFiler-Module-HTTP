/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_CONFIG_H
#define BOOKFILER_MODULE_HTTP_CONFIG_H

#define HTTPS_GET_JSON_DEBUG 1
#define HTTPS_GET_JSON_ENABLE 1
#define BOOKFILER_HTTP_CLIENT_END_DEBUG_RESPONSE 1
#define BOOKFILER_HTTP_CLIENT_CLIENT_DEBUG_URL 1
#define RSA_KEY_LENGTH 2048
#define BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE 1

#include <string>

namespace bookfiler {
namespace HTTP {

static std::string moduleName = "BookFiler Module HTTP";
static std::string moduleCode = "bookfiler::HTTP";

} // namespace HTTP
namespace certificate {

static std::string moduleName = "BookFiler Module Certificate";
static std::string moduleCode = "bookfiler::certificate";

} // namespace certificate
} // namespace bookfiler

#endif // BOOKFILER_MODULE_MYSQL_CONFIG_H
