/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpCurl.hpp"

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

ConnectionImpl::ConnectionImpl(){};
ConnectionImpl::~ConnectionImpl(){};

int ConnectionImpl::setSettingsDoc(
    std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}

int ConnectionImpl::setURL(std::string url_) {
  url = url_;
  return 0;
}
int ConnectionImpl::exec() {
  CURL *curl;
  CURLcode res;
  std::string bufferString;

  responseJSON_Doc = std::make_shared<rapidjson::Document>();

  printf("HTTPS_GET_JSON called on: %s\n", url.c_str());

  curl = curl_easy_init();
  if (!curl) {
    std::cout << "curl_easy_init ERROR" << std::endl;
    jsonReceivedSignal(responseJSON_Doc);
    return -1;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  // curl_easy_setopt(curl, CURLOPT_CAINFO,
  // "/etc/ssl/certs/ca-certificates.crt");

  /* Disable SSL/TLS verification
   * TODO: Write the SSL Certificate Manager
   */
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

  /*
   * TODO: Write the SSL Certificate Manager
   */
  // getCertificates(global);
#if CURL_SKIP_PEER_VERIFICATION
  /*
   * If you want to connect to a site who isn't using a certificate that is
   * signed by one of the certs in the CA bundle you have, you can skip the
   * verification of the server's certificate. This makes the connection
   * A LOT LESS SECURE.
   *
   * If you have a CA cert for the server stored someplace else than in the
   * default bundle, then the CURLOPT_CAPATH option might come handy for
   * you.
   */
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#if CURL_SKIP_HOSTNAME_VERIFICATION
  /*
   * If the site you're connecting to uses a different host name that what
   * they have mentioned in their server certificate's commonName (or
   * subjectAltName) fields, libcurl will refuse to connect. You can skip
   * this check, but this will make the connection less secure.
   */
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

  /* setup callbacks */
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, bookfiler::curl::writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bufferString);
  /* Perform the request, res will get the return code */
  res = curl_easy_perform(curl);
  responseJSON_Doc->Parse(bufferString.c_str());

  /* Check for errors */
  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    return -1;
  }

  jsonReceivedSignal(responseJSON_Doc);

#if HTTPS_GET_JSON_DEBUG
  std::cout << bufferString << std::endl;
#endif

  /* always cleanup */
  curl_easy_cleanup(curl);
  return 0;
}

void printJSON_value(const rapidjson::Value &a, unsigned int depth) {
  if (a.IsArray()) {
    rapidjson::SizeType n =
        a.Size(); // rapidjson uses SizeType instead of size_t.
    for (rapidjson::SizeType i = 0; i < n; i++) {
      if (a[i].IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = a[i].MemberBegin();
        printJSON_iterator(itr, ++depth);
      } else if (a[i].IsArray()) {
        const rapidjson::Value &aa = a[i];
        printJSON_value(aa, ++depth);
      }
    }
  }
}

void printJSON_iterator(rapidjson::Value::ConstMemberIterator &itr,
                        unsigned int depth) {
  static const char *kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                     "Array", "String", "Number"};
  printf("Type of member %s is %s\n", itr->name.GetString(),
         kTypeNames[itr->value.GetType()]);
  if (itr->value.GetType() == 3) {
    const rapidjson::Value &a = itr->value;
    for (rapidjson::Value::ConstMemberIterator itr2 = a.MemberBegin();
         itr2 != a.MemberEnd(); ++itr2) {
      printJSON_iterator(itr2, ++depth);
    }
  } else if (itr->value.GetType() == 4) {
    const rapidjson::Value &a = itr->value;
    printJSON_value(a, ++depth);
  }
}

} // namespace HTTP
} // namespace bookfiler
