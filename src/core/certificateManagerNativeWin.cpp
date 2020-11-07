/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#if defined(_WIN32)

// Local Project
#include "certificateManagerNativeWin.hpp"

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

ManagerNativeImpl::ManagerNativeImpl() {}
ManagerNativeImpl::~ManagerNativeImpl() {}

int ManagerNativeImpl::createX509Store() {
  std::cout << "ManagerImpl::createX509Store" << std::endl;
  int rc = 0;

  certList.clear();

  PCCERT_CONTEXT pCertContext = NULL;
  X509_STORE *storePtr = X509_STORE_new();
  if (!storePtr) {
    std::cout << "ManagerImpl::createX509Store X509_STORE_new ERROR"
              << std::endl;
    return -1;
  }

  hCertStore = CertOpenSystemStoreW(0, L"ROOT");
  if (!hCertStore) {
    std::cout << "ManagerImpl::createX509Store CertOpenSystemStoreW ERROR"
              << std::endl;
    return -1;
  }

  while (pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext)) {
    if (!pCertContext) {
      std::cout
          << "ManagerImpl::createX509Store CertEnumCertificatesInStore ERROR"
          << std::endl;
      continue;
    }

    std::shared_ptr<CertificateNativeImpl> certPtr =
        std::make_shared<CertificateNativeImpl>();
    certPtr->pCertContext = pCertContext;
    certList.push_back(certPtr);
    rc = certPtr->toX509();
    if (rc < 0) {
      std::cout << "ManagerImpl::createX509Store toX509 ERROR" << std::endl;
      continue;
    }
    rc = X509_STORE_add_cert(storePtr, certPtr->certX509);

    if (rc < 0) {
      std::cout << "ManagerImpl::createX509Store X509_STORE_add_cert ERROR"
                << std::endl;
    }
    std::cout << "certificate added:\n"
              << certPtr->getInfo() << "\n"
              << std::endl;

    // stop the certificate from automatically being deleted
    certPtr->pCertContext = nullptr;
  }

  CertCloseStore(hCertStore, 0);
  return 0;
}

int ManagerNativeImpl::addCertificate(std::shared_ptr<Certificate> certPtr) {
  int rc = 0;
  hCertStore = CertOpenSystemStoreW(0, L"ROOT");
  std::cout << "Certificate Info:\n" << certPtr->getInfo() << std::endl;
  std::shared_ptr<CertificateNativeImpl> certNativeImplPtr =
      std::static_pointer_cast<CertificateNativeImpl>(certPtr);
  rc = certNativeImplPtr->toNative();
  if (rc < 0) {
    std::cout << "ManagerImpl::addCertificate toNative ERROR" << std::endl;
    return -1;
  }

  bool status = CertAddCertificateContextToStore(
      hCertStore, certNativeImplPtr->pCertContext, CERT_STORE_ADD_NEW, NULL);
  if (!status) {
    std::cout
        << "ManagerImpl::addCertificate CertAddCertificateContextToStore ERROR"
        << std::endl;
    return -1;
  }

  CertCloseStore(hCertStore, 0);
  return 0;
}

} // namespace certificate
} // namespace bookfiler

#endif
