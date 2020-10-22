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

// Local Project
#include "certificate.hpp"
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

class ManagerImpl : public Manager {
private:
  HCERTSTORE hCertStore;
  std::shared_ptr<X509_STORE> storePtr;
  std::vector<std::shared_ptr<CertificateNativeImpl>> certList;
  std::shared_ptr<CertificateNativeImpl> certRootLocalhostPtr,
      certServerLocalhostPtr;
  std::shared_ptr<rapidjson::Value> settingsDoc;

public:
  ManagerImpl();
  ~ManagerImpl();
  int setSettingsDoc(std::shared_ptr<rapidjson::Value> settingsDoc_);
  int createX509Store();

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
  int addCertificate(std::shared_ptr<Certificate>);
  int saveCertificate(std::shared_ptr<Certificate>, std::string);
  int loadCertificate(std::shared_ptr<Certificate> &);
};

} // namespace certificate
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
