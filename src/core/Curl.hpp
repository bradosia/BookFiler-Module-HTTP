/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_CURL_H
#define BOOKFILER_MODULE_HTTP_CURL_H

// config
#include "config.hpp"

// c++17
//#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/reader.h> // rapidjson::ParseResult
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

/*
 * CURL 7.68.0
 * License: MIT/X inspired
 */
#define CURL_STATICLIB
#include <brotli/decode.h>
#include <curl/curl.h>

// Local Project
#include <BookFiler-Module-HTTP-Curl/Interface.hpp>

/*
 * bookfiler - Curl
 */
namespace bookfiler {
namespace curl {

class InitImpl : public Init {
public:
  InitImpl();
  ~InitImpl();
};

/* Writes data from HTTP request into a string buffer
 * @param ptr data address
 */
size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s);

} // namespace MySQL
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
