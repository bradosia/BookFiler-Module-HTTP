//
// CookieImpl.cpp
//
// Library: Net
// Package: HTTP
// Module:  CookieImpl
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//

// Local Project
#include "httpCookie.hpp"

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

CookieImpl::CookieImpl()
    : _version(0), _secure(false), _maxAge(-1), _httpOnly(false),
      _sameSite(SAME_SITE_NOT_SPECIFIED) {}

CookieImpl::CookieImpl(const std::string &name)
    : _version(0), _name(name), _secure(false), _maxAge(-1), _httpOnly(false),
      _sameSite(SAME_SITE_NOT_SPECIFIED) {}

CookieImpl::CookieImpl(const std::unordered_map<std::string, std::string> &nvc)
    : _version(0), _secure(false), _maxAge(-1), _httpOnly(false),
      _sameSite(SAME_SITE_NOT_SPECIFIED) {
  for (const auto &p : nvc) {
    const std::string &name = p.first;
    const std::string &value = p.second;
    if (name.compare("comment") == 0) {
      setComment(value);
    } else if (name.compare("domain") == 0) {
      setDomain(value);
    } else if (name.compare("path") == 0) {
      setPath(value);
    } else if (name.compare("priority") == 0) {
      setPriority(value);
    } else if (name.compare("max-age") == 0) {
      setMaxAge(std::stoi(value));
    } else if (name.compare("secure") == 0) {
      setSecure(true);
    } else if (name.compare("expires") == 0) {
      std::tm tm = {};
      std::istringstream ss(value);
      ss >> std::get_time(&tm, "%a, %d-%b-%Y %T %Z");
      std::chrono::time_point<std::chrono::system_clock> tp =
          std::chrono::system_clock::from_time_t(std::mktime(&tm));
      auto now = std::chrono::system_clock::now();
      setMaxAge(
          std::chrono::duration_cast<std::chrono::seconds>(tp - now).count());
    } else if (name.compare("SameSite") == 0) {
      if (value.compare("None") == 0)
        _sameSite = SAME_SITE_NONE;
      else if (value.compare("Lax") == 0)
        _sameSite = SAME_SITE_LAX;
      else if (value.compare("Strict") == 0)
        _sameSite = SAME_SITE_STRICT;
    } else if (name.compare("version") == 0) {
      setVersion(std::stoi(value));
    } else if (name.compare("HttpOnly") == 0) {
      setHttpOnly(true);
    } else {
      setName(name);
      setValue(value);
    }
  }
}

CookieImpl::CookieImpl(const std::string &name, const std::string &value)
    : _version(0), _name(name), _value(value), _secure(false), _maxAge(-1),
      _httpOnly(false), _sameSite(SAME_SITE_NOT_SPECIFIED) {}

CookieImpl::CookieImpl(const CookieImpl &cookie)
    : _version(cookie._version), _name(cookie._name), _value(cookie._value),
      _comment(cookie._comment), _domain(cookie._domain), _path(cookie._path),
      _priority(cookie._priority), _secure(cookie._secure),
      _maxAge(cookie._maxAge), _httpOnly(cookie._httpOnly),
      _sameSite(cookie._sameSite) {}

CookieImpl::~CookieImpl() {}

CookieImpl &CookieImpl::operator=(const CookieImpl &cookie) {
  if (&cookie != this) {
    _version = cookie._version;
    _name = cookie._name;
    _value = cookie._value;
    _comment = cookie._comment;
    _domain = cookie._domain;
    _path = cookie._path;
    _priority = cookie._priority;
    _secure = cookie._secure;
    _maxAge = cookie._maxAge;
    _httpOnly = cookie._httpOnly;
    _sameSite = cookie._sameSite;
  }
  return *this;
}

void CookieImpl::setVersion(int version) { _version = version; }

void CookieImpl::setName(const std::string &name) { _name = name; }

void CookieImpl::setValue(const std::string &value) { _value = value; }

void CookieImpl::setComment(const std::string &comment) { _comment = comment; }

void CookieImpl::setDomain(const std::string &domain) { _domain = domain; }

void CookieImpl::setPath(const std::string &path) { _path = path; }

void CookieImpl::setPriority(const std::string &priority) {
  _priority = priority;
}

void CookieImpl::setSecure(bool secure) { _secure = secure; }

void CookieImpl::setMaxAge(int maxAge) { _maxAge = maxAge; }

void CookieImpl::setHttpOnly(bool flag) { _httpOnly = flag; }

void CookieImpl::setSameSite(SameSite value) { _sameSite = value; }

std::string CookieImpl::toString() const {
  std::string result;
  result.reserve(256);
  result.append(_name);
  result.append("=");
  if (_version == 0) {
    // Netscape cookie
    result.append(_value);
    if (!_domain.empty()) {
      result.append("; domain=");
      result.append(_domain);
    }
    if (!_path.empty()) {
      result.append("; path=");
      result.append(_path);
    }
    if (!_priority.empty()) {
      result.append("; Priority=");
      result.append(_priority);
    }
    if (_maxAge != -1) {
      auto now = std::chrono::system_clock::now();
      now += std::chrono::seconds(_maxAge);
      auto in_time_t = std::chrono::system_clock::to_time_t(now);
      std::stringstream ss;
      ss << std::put_time(std::localtime(&in_time_t), "%a, %d-%b-%Y %T %Z");
      result.append("; expires=");
      result.append(ss.str());
    }
    switch (_sameSite) {
    case SAME_SITE_NONE:
      result.append("; SameSite=None");
      break;
    case SAME_SITE_LAX:
      result.append("; SameSite=Lax");
      break;
    case SAME_SITE_STRICT:
      result.append("; SameSite=Strict");
      break;
    case SAME_SITE_NOT_SPECIFIED:
      break;
    }
    if (_secure) {
      result.append("; secure");
    }
    if (_httpOnly) {
      result.append("; HttpOnly");
    }
  } else {
    // RFC 2109 cookie
    result.append("\"");
    result.append(_value);
    result.append("\"");
    if (!_comment.empty()) {
      result.append("; Comment=\"");
      result.append(_comment);
      result.append("\"");
    }
    if (!_domain.empty()) {
      result.append("; Domain=\"");
      result.append(_domain);
      result.append("\"");
    }
    if (!_path.empty()) {
      result.append("; Path=\"");
      result.append(_path);
      result.append("\"");
    }
    if (!_priority.empty()) {
      result.append("; Priority=\"");
      result.append(_priority);
      result.append("\"");
    }

    if (_maxAge != -1) {
      result.append("; Max-Age=\"");
      result.append(std::to_string(_maxAge));
      result.append("\"");
    }
    switch (_sameSite) {
    case SAME_SITE_NONE:
      result.append("; SameSite=None");
      break;
    case SAME_SITE_LAX:
      result.append("; SameSite=Lax");
      break;
    case SAME_SITE_STRICT:
      result.append("; SameSite=Strict");
      break;
    case SAME_SITE_NOT_SPECIFIED:
      break;
    }
    if (_secure) {
      result.append("; secure");
    }
    if (_httpOnly) {
      result.append("; HttpOnly");
    }
    result.append("; Version=\"1\"");
  }
  return result;
}

static const std::string ILLEGAL_CHARS("()[]/|\\',;");

std::string CookieImpl::escape(const std::string &str) {
  std::string result;
  //util.uriEncode(str, ILLEGAL_CHARS, result);
  return result;
}

std::string CookieImpl::unescape(const std::string &str) {
  std::string result;
  //util.uriDecode(str, result);
  return result;
}

} // namespace Net
} // namespace Poco
