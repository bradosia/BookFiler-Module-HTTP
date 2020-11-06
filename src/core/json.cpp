/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.01
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "json.hpp"

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace JSON {

std::optional<std::string> getMemberString(const rapidjson::Value &val,
                                           const char *key) {
  if (val.HasMember(key) && val[key].IsString()) {
    return val[key].GetString();
  }
  return {};
}

std::optional<int> getMemberInt(const rapidjson::Value &val, const char *key) {
  if (val.HasMember(key) && val[key].IsInt()) {
    return val[key].GetInt();
  }
  return {};
}

void printJSON_value(const rapidjson::Value &a, unsigned int depth) {
  if (a.IsArray()) {
    rapidjson::SizeType n =
        a.Size(); // rapidjson uses SizeType instead of size_t.
    for (rapidjson::SizeType i = 0; i < n; i++) {
      if (a[i].IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = a[i].MemberBegin();
        printJSON_iterator(itr, ++depth);
      } else if (a[i].IsArray()) {
        const rapidjson::Value &aa = a[i];
        printJSON_value(aa, ++depth);
      }
    }
  }
}

void printJSON_iterator(rapidjson::Value::ConstMemberIterator &itr,
                        unsigned int depth) {
  static const char *kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                     "Array", "String", "Number"};
  printf("Type of member %s is %s\n", itr->name.GetString(),
         kTypeNames[itr->value.GetType()]);
  if (itr->value.GetType() == 3) {
    const rapidjson::Value &a = itr->value;
    for (rapidjson::Value::ConstMemberIterator itr2 = a.MemberBegin();
         itr2 != a.MemberEnd(); ++itr2) {
      printJSON_iterator(itr2, ++depth);
    }
  } else if (itr->value.GetType() == 4) {
    const rapidjson::Value &a = itr->value;
    printJSON_value(a, ++depth);
  }
}

} // namespace JSON
} // namespace bookfiler
