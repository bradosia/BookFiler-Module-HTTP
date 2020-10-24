/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "certificate.hpp"

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

std::shared_ptr<std::string> CertificateImpl::getCertStr() {
  int rc = 0;

  std::unique_ptr<BIO, void (*)(BIO *)> certFile{BIO_new(BIO_s_mem()),
                                                 BIO_free_all};

  if (!certFile) {
    std::cout << moduleCode << "::CertificateImpl::getCertStr BIO ERROR"
              << std::endl;
    return nullptr;
  }

  rc = PEM_write_bio_X509(certFile.get(), certX509);

  if (rc < 0) {
    std::cout << moduleCode
              << "::Certificate::getCertStr PEM_write_bio_X509 ERROR"
              << std::endl;
    return nullptr;
  }

  BUF_MEM *mem = NULL;
  BIO_get_mem_ptr(certFile.get(), &mem);
  unsigned long err = ERR_get_error();

  if (!mem || !mem->data || !mem->length) {
    std::cout << moduleCode
              << "::Certificate::getCertStr BIO_get_mem_ptr ERROR: " << err
              << "\n"
              << std::hex << "0x" << err << std::endl;
    return nullptr;
  }

  std::shared_ptr<std::string> returnStr =
      std::make_shared<std::string>(mem->data, mem->length);

  std::cout << "============ getCertStr ============\n"
            << *returnStr << std::endl;

  return returnStr;
}
std::shared_ptr<std::string> CertificateImpl::getPrivateKeyStr() {
  std::unique_ptr<BIO, void (*)(BIO *)> keyFile{BIO_new(BIO_s_mem()),
                                                BIO_free_all};

  if (!keyFile) {
    std::cout << moduleCode
              << "::CertificateImpl::getPrivateKeyStr BIO_new ERROR"
              << std::endl;
    return nullptr;
  }

  int ret = PEM_write_bio_PrivateKey(keyFile.get(), privateKey, nullptr,
                                     nullptr, 0, nullptr, nullptr);

  if (ret < 0) {
    std::cout
        << moduleCode
        << "::CertificateImpl::getPrivateKeyStr PEM_write_bio_PrivateKey ERROR"
        << std::endl;
    return nullptr;
  }

  BUF_MEM *mem = NULL;
  BIO_get_mem_ptr(keyFile.get(), &mem);
  unsigned long err = ERR_get_error();

  if (!mem || !mem->data || !mem->length) {
    std::cout << moduleCode
              << "::CertificateImpl::getPrivateKeyStr BIO_get_mem_ptr ERROR: "
              << err << "\n"
              << std::hex << "0x" << err << std::endl;
    return nullptr;
  }

  std::shared_ptr<std::string> returnStr =
      std::make_shared<std::string>(mem->data, mem->length);

  std::cout << "============ getPrivateKeyStr ============\n"
            << *returnStr << std::endl;

  return returnStr;
}

std::shared_ptr<std::string> CertificateImpl::getDhStr() {
  std::unique_ptr<BIO, void (*)(BIO *)> dhFile{BIO_new(BIO_s_mem()),
                                               BIO_free_all};

  if (!dhFile) {
    std::cout << moduleCode
              << "::CertificateImpl::getPrivateKeyStr BIO_new ERROR"
              << std::endl;
    return nullptr;
  }

  int ret = PEM_write_bio_DHparams(dhFile.get(), dhKey);

  if (ret < 0) {
    std::cout << moduleCode
              << "::CertificateImpl::getDhStr PEM_write_bio_DHparams ERROR"
              << std::endl;
    return nullptr;
  }

  BUF_MEM *mem = NULL;
  BIO_get_mem_ptr(dhFile.get(), &mem);
  unsigned long err = ERR_get_error();

  if (!mem || !mem->data || !mem->length) {
    std::cout << moduleCode
              << "::CertificateImpl::getPrivateKeyStr BIO_get_mem_ptr ERROR: "
              << err << "\n"
              << std::hex << "0x" << err << std::endl;
    return nullptr;
  }

  std::shared_ptr<std::string> returnStr =
      std::make_shared<std::string>(mem->data, mem->length);

  std::cout << "============ getDhStr ============\n"
            << *returnStr << std::endl;

  return returnStr;
}

int CertificateImpl::addExt(int nid, const char *value) {
  X509_EXTENSION *ex;
  X509V3_CTX ctx;
  // This sets the 'context' of the extensions. No configuration database
  X509V3_set_ctx_nodb(&ctx);
  // Issuer and subject certs: both the target since it is self signed, no
  // request and no CRL
  X509V3_set_ctx(&ctx, certX509, certX509, NULL, NULL, 0);
  ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
  if (!ex)
    return 0;

  X509_add_ext(certX509, ex, -1);
  X509_EXTENSION_free(ex);
  return 1;
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

std::string CertificateNativeImpl::getNativeName() { return ""; }

} // namespace certificate
} // namespace bookfiler
