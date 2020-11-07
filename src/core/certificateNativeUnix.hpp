/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#if defined(__linux__)

#ifndef BOOKFILER_MODULE_HTTP_CERTIFICATE_NATIVE_H
#define BOOKFILER_MODULE_HTTP_CERTIFICATE_NATIVE_H

// config
#include "config.hpp"

// C++17
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
#include "certificate.hpp"

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

class CertificateNativeImpl : public CertificateImpl {
public:
  CertificateNativeImpl();
  ~CertificateNativeImpl();
  int toNative();
  int toX509();
  std::string getNativeName();
};

} // namespace certificate
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_CERTIFICATE_NATIVE_H

#endif
