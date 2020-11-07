/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#if defined(__linux__)

// Local Project
#include "certificateNativeUnix.hpp"

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

CertificateNativeImpl::CertificateNativeImpl() {}

CertificateNativeImpl::~CertificateNativeImpl() {
  CertFreeCertificateContext(pCertContext);
}

int CertificateNativeImpl::toNative() {
  return 0;
}

int CertificateNativeImpl::toX509() {
  return 0;
}

std::string CertificateNativeImpl::getNativeName() { return ""; }

} // namespace certificate
} // namespace bookfiler

#endif
