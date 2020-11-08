/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "Template.hpp"

/*
 * bookfiler - Template
 */
namespace bookfiler {

TemplateImpl::TemplateImpl() { data.set("test", "value test"); }
TemplateImpl::~TemplateImpl() {}
int TemplateImpl::setSettingsDoc(
    std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}
int TemplateImpl::readFile(std::string path_) {
  templateStr = readFile(path_);
  return 0;
}
int TemplateImpl::mergeData(object) { return 0; }
int TemplateImpl::mergeData(rapidjson::Document &) { return 0; }
int TemplateImpl::parse() {
  kainjow::mustache::mustache tmpl{templateStr};
  parsedStr = tmpl.render(data);
  return 0;
}

int TemplateImpl::addContext(std::string key, std::string value) {
  data.set(key, key);
  return 0;
}

std::string TemplateImpl::toString() { return parsedStr; }

} // namespace bookfiler
