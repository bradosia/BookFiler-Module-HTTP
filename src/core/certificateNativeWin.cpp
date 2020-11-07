/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#if defined(_WIN32)

// Local Project
#include "certificateNativeWin.hpp"

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
  unsigned char *certBuffer = nullptr;
  DWORD certLen = i2d_X509(certX509, &certBuffer);
  std::cout
      << "ManagerImpl::addCertificate CertCreateCertificateContext length="
      << certLen << std::endl;
  pCertContext =
      ::CertCreateCertificateContext(X509_ASN_ENCODING, certBuffer, certLen);
  if (pCertContext == nullptr) {
    std::cout
        << "CertificateNativeImpl::toNative CertCreateCertificateContext ERROR"
        << std::endl;
    return -1;
  }
  return 0;
}

int CertificateNativeImpl::toX509() {
  certX509 =
      d2i_X509(NULL, (const unsigned char **)&pCertContext->pbCertEncoded,
               pCertContext->cbCertEncoded);
  if (certX509 == nullptr) {
    std::cout << "CertificateNativeImpl::toX509 d2i_X509 ERROR" << std::endl;
    return -1;
  }
  return 0;
}

std::string CertificateNativeImpl::getNativeName() { return ""; }

} // namespace certificate
} // namespace bookfiler

#endif
