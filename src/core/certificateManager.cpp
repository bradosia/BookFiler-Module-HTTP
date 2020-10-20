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

int ManagerImpl::newCertificate(
    std::shared_ptr<Certificate> &certPtr,
    std::shared_ptr<rapidjson::Document> settingsDoc) {
  int daysValid = 365;
  std::string countryStr = "C";
  std::string companyStr = "O";
  std::string commonNameStr = "CN";
  std::string ouStr = "OU";
  std::string cityStr = "L";
  std::string stateStr = "S";

  int rc = 0;

  /* Get settings from JSON document */
  if (!settingsDoc->IsObject()) {
    std::cout
        << moduleCode
        << "::ManagerImpl::newCertificate WARNING:\nSettings document invalid"
        << std::endl;
  } else {
    char memberName01[] = "daysValid";
    if (settingsDoc->HasMember(memberName01) &&
        (*settingsDoc)[memberName01].IsInt()) {
      daysValid = (*settingsDoc)[memberName01].GetInt();
    }
    char memberName02[] = "country";
    if (settingsDoc->HasMember(memberName02) &&
        (*settingsDoc)[memberName02].IsString()) {
      countryStr = (*settingsDoc)[memberName02].GetString();
    }
    char memberName03[] = "company";
    if (settingsDoc->HasMember(memberName03) &&
        (*settingsDoc)[memberName03].IsString()) {
      companyStr = (*settingsDoc)[memberName03].GetString();
    }
    char memberName04[] = "commonName";
    if (settingsDoc->HasMember(memberName04) &&
        (*settingsDoc)[memberName04].IsString()) {
      commonNameStr = (*settingsDoc)[memberName04].GetString();
    }
    char memberName05[] = "OU";
    if (settingsDoc->HasMember(memberName05) &&
        (*settingsDoc)[memberName05].IsString()) {
      ouStr = (*settingsDoc)[memberName05].GetString();
    }
    char memberName06[] = "city";
    if (settingsDoc->HasMember(memberName06) &&
        (*settingsDoc)[memberName06].IsString()) {
      cityStr = (*settingsDoc)[memberName06].GetString();
    }
    char memberName07[] = "state";
    if (settingsDoc->HasMember(memberName07) &&
        (*settingsDoc)[memberName07].IsString()) {
      stateStr = (*settingsDoc)[memberName07].GetString();
    }
  }

  // Create certificate
  std::shared_ptr<CertificateImpl> certImplPtr =
      std::make_shared<CertificateImpl>();
  certImplPtr->certX509 = X509_new();
  certImplPtr->privateKey = EVP_PKEY_new();
  certImplPtr->dhKey = DH_new();

  // Generate RSA Key
  std::unique_ptr<RSA, void (*)(RSA *)> rsaPtr{RSA_new(), RSA_free};
  std::unique_ptr<BIGNUM, void (*)(BIGNUM *)> bigNumberPtr{BN_new(), BN_free};
  BN_set_word(bigNumberPtr.get(), RSA_F4);
  rc = RSA_generate_key_ex(rsaPtr.get(), RSA_KEY_LENGTH, bigNumberPtr.get(),
                           nullptr);
  if (rc < 0) {
    std::cout << moduleCode
              << "::ManagerImpl::newCertificate DH_generate_parameters_ex ERROR"
              << std::endl;
    return -1;
  }
  EVP_PKEY_assign(certImplPtr->privateKey, EVP_PKEY_RSA,
                  reinterpret_cast<char *>(rsaPtr.release()));

  // Generate DH Key
  if (!certImplPtr->dhKey) {
    std::cout << moduleCode << "::ManagerImpl::newCertificate DH_new ERROR"
              << std::endl;
  }
  rc = DH_generate_parameters_ex(certImplPtr->dhKey, 2048, 2, nullptr);
  if (rc < 0) {
    std::cout << moduleCode
              << "::ManagerImpl::newCertificate DH_generate_parameters_ex ERROR"
              << std::endl;
    return -1;
  }
  EVP_PKEY *evpDhKey = EVP_PKEY_new();
  EVP_PKEY_assign_DH(evpDhKey, certImplPtr->dhKey);

  // The RSA structure will be automatically freed when the EVP_PKEY
  // structure is freed.
  ASN1_INTEGER_set(X509_get_serialNumber(certImplPtr->certX509), 5);

  X509_gmtime_adj(X509_get_notBefore(certImplPtr->certX509), 0); // now
  X509_gmtime_adj(X509_get_notAfter(certImplPtr->certX509),
                  daysValid * 24 * 3600); // accepts secs
  X509_set_pubkey(certImplPtr->certX509, certImplPtr->privateKey);
  X509_set_version(certImplPtr->certX509, 2);

  X509_name_st *name = X509_get_subject_name(certImplPtr->certX509);

  const unsigned char *country =
      reinterpret_cast<const unsigned char *>(countryStr.c_str());
  const unsigned char *company =
      reinterpret_cast<const unsigned char *>(companyStr.c_str());
  const unsigned char *commonName =
      reinterpret_cast<const unsigned char *>(commonNameStr.c_str());
  const unsigned char *ou =
      reinterpret_cast<const unsigned char *>(ouStr.c_str());
  const unsigned char *city =
      reinterpret_cast<const unsigned char *>(cityStr.c_str());
  const unsigned char *state =
      reinterpret_cast<const unsigned char *>(stateStr.c_str());

  X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, country, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, company, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, commonName, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "OU", MBSTRING_ASC, ou, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, city, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "S", MBSTRING_ASC, state, -1, -1, 0);

  X509_set_issuer_name(certImplPtr->certX509, name);

  // Add various extensions: standard extensions
  certImplPtr->addExt(NID_basic_constraints, "critical,CA:TRUE");
  certImplPtr->addExt(NID_key_usage, "critical,keyCertSign,cRLSign");
  certImplPtr->addExt(NID_subject_key_identifier, "hash");
  certImplPtr->addExt(NID_subject_alt_name, "DNS:localhost");
  certImplPtr->addExt(NID_ext_key_usage, "critical,codeSigning,1.2.3.4");
  // certImplPtr->addExt(NID_netscape_cert_type, "sslCA");
  // certImplPtr->addExt(NID_netscape_comment, "example comment extension");

  X509_sign(certImplPtr->certX509, certImplPtr->privateKey,
            EVP_sha256()); // some hash type here

  // return
  certPtr = std::dynamic_pointer_cast<Certificate>(certImplPtr);

  return 0;
}

int ManagerImpl::newRequest(std::shared_ptr<Certificate> &certPtr,
                            std::shared_ptr<rapidjson::Document> settingsDoc) {
  int daysValid = 365;
  std::string countryStr = "C";
  std::string companyStr = "O";
  std::string commonNameStr = "CN";

  int rc = 0;

  /* Get settings from JSON document */
  if (!settingsDoc->IsObject()) {
    std::cout
        << moduleCode
        << "::ManagerImpl::newCertificate WARNING:\nSettings document invalid"
        << std::endl;
  } else {
    // nothing
  }

  // Create certificate
  std::shared_ptr<CertificateImpl> certImplPtr =
      std::make_shared<CertificateImpl>();
  certImplPtr->certReqX509 = X509_REQ_new();
  certImplPtr->privateKey = EVP_PKEY_new();
  certImplPtr->dhKey = DH_new();

  // Generate RSA Key
  std::unique_ptr<RSA, void (*)(RSA *)> rsaPtr{RSA_new(), RSA_free};
  std::unique_ptr<BIGNUM, void (*)(BIGNUM *)> bigNumberPtr{BN_new(), BN_free};
  BN_set_word(bigNumberPtr.get(), RSA_F4);
  rc = RSA_generate_key_ex(rsaPtr.get(), RSA_KEY_LENGTH, bigNumberPtr.get(),
                           nullptr);
  if (rc < 0) {
    std::cout << moduleCode
              << "::ManagerImpl::newCertificate DH_generate_parameters_ex ERROR"
              << std::endl;
    return -1;
  }
  EVP_PKEY_assign(certImplPtr->privateKey, EVP_PKEY_RSA,
                  reinterpret_cast<char *>(rsaPtr.release()));

  // Generate DH Key
  if (!certImplPtr->dhKey) {
    std::cout << moduleCode << "::ManagerImpl::newCertificate DH_new ERROR"
              << std::endl;
  }
  rc = DH_generate_parameters_ex(certImplPtr->dhKey, 2048, 2, nullptr);
  if (rc < 0) {
    std::cout << moduleCode
              << "::ManagerImpl::newCertificate DH_generate_parameters_ex ERROR"
              << std::endl;
    return -1;
  }
  EVP_PKEY *evpDhKey = EVP_PKEY_new();
  EVP_PKEY_assign_DH(evpDhKey, certImplPtr->dhKey);

  // The RSA structure will be automatically freed when the EVP_PKEY
  // structure is freed.
  X509_REQ_set_pubkey(certImplPtr->certReqX509, certImplPtr->privateKey);
  X509_REQ_set_version(certImplPtr->certReqX509, 2);

  // return
  certPtr = std::dynamic_pointer_cast<Certificate>(certImplPtr);

  return 0;
}

int ManagerImpl::signRequest(std::shared_ptr<Certificate> certPtr,
                             std::shared_ptr<Certificate> certCAPtr,
                             std::shared_ptr<rapidjson::Document> settingsDoc) {
  int daysValid = 365;
  std::string countryStr = "C";
  std::string companyStr = "O";
  std::string commonNameStr = "CN";
  std::string ouStr = "OU";
  std::string cityStr = "L";
  std::string stateStr = "S";

  int rc = 0;

  /* Get settings from JSON document */
  if (!settingsDoc->IsObject()) {
    std::cout
        << moduleCode
        << "::ManagerImpl::newCertificate WARNING:\nSettings document invalid"
        << std::endl;
  } else {
    char memberName01[] = "daysValid";
    if (settingsDoc->HasMember(memberName01) &&
        (*settingsDoc)[memberName01].IsInt()) {
      daysValid = (*settingsDoc)[memberName01].GetInt();
    }
    char memberName02[] = "country";
    if (settingsDoc->HasMember(memberName02) &&
        (*settingsDoc)[memberName02].IsString()) {
      countryStr = (*settingsDoc)[memberName02].GetString();
    }
    char memberName03[] = "company";
    if (settingsDoc->HasMember(memberName03) &&
        (*settingsDoc)[memberName03].IsString()) {
      companyStr = (*settingsDoc)[memberName03].GetString();
    }
    char memberName04[] = "commonName";
    if (settingsDoc->HasMember(memberName04) &&
        (*settingsDoc)[memberName04].IsString()) {
      commonNameStr = (*settingsDoc)[memberName04].GetString();
    }
    char memberName05[] = "OU";
    if (settingsDoc->HasMember(memberName05) &&
        (*settingsDoc)[memberName05].IsString()) {
      ouStr = (*settingsDoc)[memberName05].GetString();
    }
    char memberName06[] = "city";
    if (settingsDoc->HasMember(memberName06) &&
        (*settingsDoc)[memberName06].IsString()) {
      cityStr = (*settingsDoc)[memberName06].GetString();
    }
    char memberName07[] = "state";
    if (settingsDoc->HasMember(memberName07) &&
        (*settingsDoc)[memberName07].IsString()) {
      stateStr = (*settingsDoc)[memberName07].GetString();
    }
  }

  std::shared_ptr<CertificateImpl> certImplPtr =
      std::static_pointer_cast<CertificateImpl>(certPtr);
  std::shared_ptr<CertificateImpl> certCAImplPtr =
      std::static_pointer_cast<CertificateImpl>(certCAPtr);

  /*
  PEM_write_PrivateKey(stdout, certCAImplPtr->privateKey, NULL, NULL, 0, NULL,
  NULL); PEM_write_PUBKEY(stdout, certCAImplPtr->privateKey);
  */

  certImplPtr->certX509 = X509_new();

  X509_set_version(certImplPtr->certX509, 3);
  ASN1_INTEGER_set(X509_get_serialNumber(certImplPtr->certX509), 1024);
  X509_gmtime_adj(X509_get_notBefore(certImplPtr->certX509), 0); // now
  X509_gmtime_adj(X509_get_notAfter(certImplPtr->certX509),
                  daysValid * 24 * 3600); // accepts secs

  X509_set_pubkey(certImplPtr->certX509, certImplPtr->privateKey);
  X509_set_issuer_name(certImplPtr->certX509,
                       X509_get_subject_name(certCAImplPtr->certX509));

  X509_name_st *name = X509_get_subject_name(certImplPtr->certX509);

  const unsigned char *country =
      reinterpret_cast<const unsigned char *>(countryStr.c_str());
  const unsigned char *company =
      reinterpret_cast<const unsigned char *>(companyStr.c_str());
  const unsigned char *commonName =
      reinterpret_cast<const unsigned char *>(commonNameStr.c_str());
  const unsigned char *ou =
      reinterpret_cast<const unsigned char *>(ouStr.c_str());
  const unsigned char *city =
      reinterpret_cast<const unsigned char *>(cityStr.c_str());
  const unsigned char *state =
      reinterpret_cast<const unsigned char *>(stateStr.c_str());

  X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, country, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, company, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, commonName, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "OU", MBSTRING_ASC, ou, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, city, -1, -1, 0);
  X509_NAME_add_entry_by_txt(name, "S", MBSTRING_ASC, state, -1, -1, 0);

  X509_set_subject_name(certImplPtr->certX509, name);

  // Add usual cert stuff
  X509_EXTENSION *ex = nullptr;

  ex = X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage,
                           "digitalSignature, keyEncipherment, keyAgreement");
  X509_add_ext(certImplPtr->certX509, ex, -1);
  X509_EXTENSION_free(ex);

  // TLS server
  ex = X509V3_EXT_conf_nid(NULL, NULL, NID_ext_key_usage,
                           "TLS Web Server Authentication");
  X509_add_ext(certImplPtr->certX509, ex, -1);
  X509_EXTENSION_free(ex);

  ex = X509V3_EXT_conf_nid(NULL, NULL, NID_subject_alt_name, "DNS:localhost");
  X509_add_ext(certImplPtr->certX509, ex, -1);
  X509_EXTENSION_free(ex);

  rc =
      X509_sign(certImplPtr->certX509, certCAImplPtr->privateKey, EVP_sha256());
  if (rc < 0) {
    std::cout << moduleCode << "::ManagerImpl::signRequest X509_sign ERROR"
              << std::endl;
    return -1;
  }

  // return
  certServerLocalhostPtr =
      std::dynamic_pointer_cast<CertificateNativeImpl>(certImplPtr);

  return 0;
}

int ManagerImpl::newCertRootLocalhost(
    std::shared_ptr<Certificate> &certPtr,
    std::shared_ptr<rapidjson::Document> settingsDoc) {
  // some macro options
  boost::ignore_unused(settingsDoc);

  int rc = 0;
  std::shared_ptr<rapidjson::Document> settingsDoc2 =
      std::make_shared<rapidjson::Document>();
  settingsDoc2->SetObject();
  rapidjson::Value daysValidValue(2000);
  settingsDoc2->AddMember("daysValid", daysValidValue,
                          settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("country", "US", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("company", "BookFiler", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("commonName", "BookFiler Certificate Authority",
                          settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("OU", "bookfiler.com", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("city", "San Jose", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("state", "California", settingsDoc2->GetAllocator());
  rc = newCertificate(certPtr, settingsDoc2);
  if (rc < 0) {
    return -1;
  }

  certRootLocalhostPtr =
      std::static_pointer_cast<CertificateNativeImpl>(certPtr);
  return 0;
}

int ManagerImpl::newCertServerLocalhost(
    std::shared_ptr<Certificate> &certPtr,
    std::shared_ptr<rapidjson::Document> settingsDoc) {
  // some macro options
  boost::ignore_unused(settingsDoc);

  int rc = 0;
  std::shared_ptr<rapidjson::Document> settingsDoc2 =
      std::make_shared<rapidjson::Document>();
  settingsDoc2->SetObject();
  rapidjson::Value daysValidValue(2000);
  settingsDoc2->AddMember("daysValid", daysValidValue,
                          settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("country", "US", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("company", "BookFiler", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("commonName", "localhost",
                          settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("OU", "localhost", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("city", "San Jose", settingsDoc2->GetAllocator());
  settingsDoc2->AddMember("state", "California", settingsDoc2->GetAllocator());
  rc = newRequest(certPtr, settingsDoc2);
  if (rc < 0) {
    std::cout << moduleCode
              << "::ManagerImpl::newCertServerLocalhost newRequest ERROR"
              << std::endl;
    return -1;
  }
  std::cout << moduleCode << "::ManagerImpl::newCertServerLocalhost FLAG1"
            << std::endl;
  signRequest(certPtr, certRootLocalhostPtr, settingsDoc2);

  certServerLocalhostPtr =
      std::static_pointer_cast<CertificateNativeImpl>(certPtr);
  return 0;
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

int ManagerImpl::saveCertificate(std::shared_ptr<Certificate> certPtr,
                                 std::string fileName) {
  return 0;
}

} // namespace certificate
} // namespace bookfiler
