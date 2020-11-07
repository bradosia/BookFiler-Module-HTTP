/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#if defined(__linux__)

// Local Project
#include "certificateManagerNativeLinux.hpp"

/*
 * bookfiler - certificate
 */
namespace bookfiler {
namespace certificate {

ManagerNativeImpl::ManagerNativeImpl() {}
ManagerNativeImpl::~ManagerNativeImpl() {}

int ManagerNativeImpl::createX509Store() {
  return 0;
}

int ManagerNativeImpl::addCertificate(std::shared_ptr<Certificate> certPtr) {
  return 0;
}

} // namespace certificate
} // namespace bookfiler

#endif
