/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_CERTIFICATE_MANAGER_H
#define BOOKFILER_MODULE_CERTIFICATE_MANAGER_H

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
#include <string>
#include <thread>
#include <unordered_map>
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

/* ssl, win, crypt
 */
#define _WINSOCKAPI_
#include <windows.h>
#include <winsock2.h>

#include <cryptuiapi.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <stdio.h>
#include <wincrypt.h>

// Local Project
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

#define MY_ENCODING_TYPE (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

class CertificateImpl : public Certificate {
public:
  X509 *certX509 = nullptr;
  EVP_PKEY *privateKey = nullptr;
  CertificateImpl();
  ~CertificateImpl();
  std::string getInfo();
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

class ManagerImpl : public Manager {
private:
  HCERTSTORE hCertStore;
  std::shared_ptr<X509_STORE> storePtr;
  std::vector<std::shared_ptr<CertificateNativeImpl>> certList;

public:
  ManagerImpl();
  ~ManagerImpl();
  int createX509Store();
  bool generateX509(const std::string &certFileName,
                    const std::string &keyFileName, long daysValid,
                    std::shared_ptr<Certificate> &);
  int addCertificate(std::shared_ptr<Certificate>);
};

} // namespace certificate
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
