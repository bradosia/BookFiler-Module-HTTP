/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_JSON_H
#define BOOKFILER_MODULE_HTTP_JSON_H

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

// Local Project
#define BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE 1
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace JSON {

std::optional<std::string> getMemberString(const rapidjson::Value &val,
                                           const char *key);
std::optional<int> getMemberInt(const rapidjson::Value &val, const char *key);

/* Unused JSON utilities that demonstrate how to traverse JSON documents
 */
void printJSON_value(const rapidjson::Value &a, unsigned int depth);
void printJSON_iterator(rapidjson::Value::ConstMemberIterator &itr,
                        unsigned int depth);

} // namespace JSON
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
