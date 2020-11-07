/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#if defined(_WIN32)

#ifndef BOOKFILER_MODULE_HTTP_CERTIFICATE_MANAGER_NATIVE_H
#define BOOKFILER_MODULE_HTTP_CERTIFICATE_MANAGER_NATIVE_H

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

// Local Project
#include "certificateManager.hpp"

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

class ManagerNativeImpl : public ManagerImpl {
private:
  HCERTSTORE hCertStore;

public:
  ManagerNativeImpl();
  ~ManagerNativeImpl();
  int createX509Store();
  int addCertificate(std::shared_ptr<Certificate>);
};

} // namespace certificate
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_HTTP_CERTIFICATE_MANAGER_NATIVE_H

#endif
