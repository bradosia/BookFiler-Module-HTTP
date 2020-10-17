/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "certificateManager.hpp"

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

CertificateImpl::CertificateImpl() {}

CertificateImpl::~CertificateImpl() {
  X509_free(certX509);
  EVP_PKEY_free(privateKey);
}

std::string CertificateImpl::getInfo() {
  std::string returnStr;
  char *subj = X509_NAME_oneline(X509_get_subject_name(certX509), NULL, 0);
  char *issuer = X509_NAME_oneline(X509_get_issuer_name(certX509), NULL, 0);
  returnStr.append("subject name: ")
      .append(subj)
      .append("\nissuer name: ")
      .append(issuer);
  return returnStr;
}

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
std::string CertificateNativeImpl::getNativeName() {}

ManagerImpl::ManagerImpl() {}

ManagerImpl::~ManagerImpl() {}

int ManagerImpl::createX509Store() {
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

  hCertStore = CertOpenSystemStoreW(NULL, L"ROOT");
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

bool ManagerImpl::generateX509(const std::string &certFileName,
                               const std::string &keyFileName, long daysValid,
                               std::shared_ptr<Certificate> &certPtr) {
  bool result = false;

  std::unique_ptr<BIO, void (*)(BIO *)> certFile{
      BIO_new(BIO_s_mem()), BIO_free_all};
  std::unique_ptr<BIO, void (*)(BIO *)> keyFile{
      BIO_new(BIO_s_mem()), BIO_free_all};

  if (certFile && keyFile) {
    std::unique_ptr<RSA, void (*)(RSA *)> rsa{RSA_new(), RSA_free};
    std::unique_ptr<BIGNUM, void (*)(BIGNUM *)> bn{BN_new(), BN_free};

    BN_set_word(bn.get(), RSA_F4);
    int rsa_ok =
        RSA_generate_key_ex(rsa.get(), RSA_KEY_LENGTH, bn.get(), nullptr);

    if (rsa_ok == 1) {
      // --- cert generation ---
      X509 *certX509 = X509_new();
      EVP_PKEY *privateKey = EVP_PKEY_new();

      // The RSA structure will be automatically freed when the EVP_PKEY
      // structure is freed.
      EVP_PKEY_assign(privateKey, EVP_PKEY_RSA,
                      reinterpret_cast<char *>(rsa.release()));
      ASN1_INTEGER_set(X509_get_serialNumber(certX509),
                       1); // serial number

      X509_gmtime_adj(X509_get_notBefore(certX509), 0); // now
      X509_gmtime_adj(X509_get_notAfter(certX509),
                      daysValid * 24 * 3600); // accepts secs

      X509_set_pubkey(certX509, privateKey);

      // 1 -- X509_NAME may disambig with wincrypt.h
      // 2 -- DO NO FREE the name internal pointer
      X509_name_st *name = X509_get_subject_name(certX509);

      const unsigned char country[] = "RU";
      const unsigned char company[] = "MyCompany, PLC";
      const unsigned char common_name[] = "localhost";

      X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, country, -1, -1, 0);
      X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, company, -1, -1, 0);
      X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, common_name, -1, -1,
                                 0);

      X509_set_issuer_name(certX509, name);
      X509_sign(certX509, privateKey,
                EVP_sha256()); // some hash type here

      int ret = PEM_write_bio_PrivateKey(keyFile.get(), privateKey, nullptr,
                                         nullptr, 0, nullptr, nullptr);
      int ret2 = PEM_write_bio_X509(certFile.get(), certX509);

      std::shared_ptr<CertificateImpl> certImplPtr =
          std::make_shared<CertificateImpl>();
      certImplPtr->certX509 = certX509;
      certImplPtr->privateKey = privateKey;

      certPtr = std::dynamic_pointer_cast<Certificate>(certImplPtr);

      result = (ret == 1) && (ret2 == 1); // OpenSSL return codes
    }
  }

  return result;
}

int ManagerImpl::addCertificate(std::shared_ptr<Certificate> certPtr) {
  int rc = 0;
  hCertStore = CertOpenSystemStoreW(NULL, L"ROOT");
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
