/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_CERTIFICATE_H
#define BOOKFILER_MODULE_CERTIFICATE_H

// config
#include "config.hpp"

// C++17
//#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <thread>
#include <map>
#include <utility>
#include <vector>

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

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/signals2.hpp>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>

/* win API
 */
#include <winsock2.h>

#define _WINSOCKAPI_
#include <windows.h>

#include <Prsht.h>

/* crypto
 */
#include <cryptuiapi.h>
#include <wincrypt.h>

/* openssl
 * version: 1.1.1.h-1
 * License: custom:BSD
 */
#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/buffer.h>
#include <openssl/conf.h>
#include <openssl/dh.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

// Local Project
#define BOOKFILER_MODULE_HTTP_BOOST_BEAST_EXPOSE 1
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

class CertificateImpl : public Certificate {
public:
  X509 *certX509 = nullptr;
  X509_REQ *certReqX509 = nullptr;
  EVP_PKEY *privateKey = nullptr;
  DH *dhKey = nullptr;
  CertificateImpl();
  ~CertificateImpl();
  std::string getInfo();
  std::shared_ptr<std::string> getCertStr();
  std::shared_ptr<std::string> getPrivateKeyStr();
  std::shared_ptr<std::string> getDhStr();
  int addExt(int, const char *);
};

class CertificateNativeImpl : public CertificateImpl {
public:
  PCCERT_CONTEXT pCertContext = nullptr;
  CertificateNativeImpl();
  ~CertificateNativeImpl();
  int toNative();
  int toX509();
  std::string getNativeName();
};

} // namespace certificate
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
