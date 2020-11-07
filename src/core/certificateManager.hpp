/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_CERTIFICATE_MANAGER_H
#define BOOKFILER_MODULE_HTTP_CERTIFICATE_MANAGER_H

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

// Local Project
#include "certificate.hpp"

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

class ManagerImpl : public Manager {
private:
  std::shared_ptr<X509_STORE> storePtr;
  std::shared_ptr<CertificateNativeImpl> certRootLocalhostPtr,
      certServerLocalhostPtr;
  std::shared_ptr<rapidjson::Value> settingsDoc;

protected:
  std::vector<std::shared_ptr<CertificateNativeImpl>> certList;

public:
  ManagerImpl();
  ~ManagerImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value> settingsDoc_);

  /* Creates a new certificate using the settings specified in the json
   * document.
   */
  int newCertificate(std::shared_ptr<Certificate> &,
                     std::shared_ptr<rapidjson::Document>);
  int newRequest(std::shared_ptr<Certificate> &,
                 std::shared_ptr<rapidjson::Document>);
  int signRequest(std::shared_ptr<Certificate>, std::shared_ptr<Certificate>,
                  std::shared_ptr<rapidjson::Document>);
  /* Creates a root certificate for bookfiler local apps */
  int newCertRootLocalhost(std::shared_ptr<Certificate> &,
                           std::shared_ptr<rapidjson::Document>);
  /* Creates a server certificate for bookfiler local apps signed by the root
   * certificate */
  int newCertServerLocalhost(std::shared_ptr<Certificate> &,
                             std::shared_ptr<rapidjson::Document>);
  int saveCertificate(std::shared_ptr<Certificate>, std::string);
  int loadCertificate(std::shared_ptr<Certificate> &);

  // Requires some native implementation
  virtual int createX509Store() = 0;
  virtual int addCertificate(std::shared_ptr<Certificate>) = 0;
};

} // namespace certificate
} // namespace bookfiler

// Local Project
#if defined(_WIN32)
#include "certificateManagerNativeWin.hpp"
#elif defined(__linux__)
#include "certificateManagerNativeLinux.hpp"
#endif

#endif
// end BOOKFILER_MODULE_HTTP_CERTIFICATE_MANAGER_H
