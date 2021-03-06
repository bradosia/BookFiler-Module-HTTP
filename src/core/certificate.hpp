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
#include <map>
#include <memory>
#include <stdio.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>

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
  virtual int toNative() = 0;
  virtual int toX509() = 0;
  virtual std::string getNativeName() = 0;
};

} // namespace certificate
} // namespace bookfiler

// Local Project
#if defined(_WIN32)
#include "certificateNativeWin.hpp"
#elif defined(__linux__)
#include "certificateNativeUnix.hpp"
#endif

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
