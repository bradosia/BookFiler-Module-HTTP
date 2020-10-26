/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "httpClient.hpp"

/*
 * bookfiler - MySQL
 */
namespace bookfiler {
namespace HTTP {

ClientImpl::ClientImpl() {
  urlPtr = std::make_shared<UrlImpl>();
  method = "GET";
};
ClientImpl::ClientImpl(
    std::unordered_map<std::string, newClientVariantType> map) {
  urlPtr = std::make_shared<UrlImpl>();
  for (auto val : map) {
    if (int *val_ = std::get_if<int>(&val.second)) {
    } else if (double *val_ = std::get_if<double>(&val.second)) {
    } else if (std::string *val_ = std::get_if<std::string>(&val.second)) {
      if (val.first == "method") {
        method = *val_;
      } else if (val.first == "host") {
        urlPtr->set_encoded_host(*val_);
      } else if (val.first == "path") {
        urlPtr->set_encoded_path(*val_);
      } else if (val.first == "scheme") {
        urlPtr->set_scheme(*val_);
      } else if (val.first == "query") {
        urlPtr->set_query(*val_);
      }
    }
  }
  if (urlPtr->scheme().empty()) {
    if (method == "GET") {
      urlPtr->set_scheme("https");
    }
  }
  std::cout << "urlPtr->encoded_url: " << urlPtr->encoded_url() << std::endl;
};
ClientImpl::~ClientImpl(){};

int ClientImpl::setSettingsDoc(std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}

int ClientImpl::setURL(std::string url_) {
  urlPtr->set_encoded_url(url_);
  return 0;
}
int ClientImpl::setFields(
    std::shared_ptr<std::unordered_map<std::string, std::string>>
        fieldsMapPtr) {
  urlPtr->setFields(fieldsMapPtr);
  return 0;
}

int ClientImpl::setHeaders(
    std::shared_ptr<std::unordered_map<std::string, std::string>>
        headersMapPtr_) {
  headersMapPtr = headersMapPtr_;
  return 0;
}

int ClientImpl::setMethod(std::string method_) {
  method = method_;
  return 0;
}

int ClientImpl::exec() {
  CURL *curlHandle;
  CURLcode res;
  std::string bufferString, CaInfoPath, fieldsStr;
  bool skipPeerVerification, skipHostnameVerification;
  skipPeerVerification = skipHostnameVerification = false;

  /* Get settings from JSON document */
  if (!settingsDoc->IsObject()) {
    std::cout << moduleCode
              << "::ClientImpl::exec ERROR:\nSettings document invalid"
              << std::endl;
    return -1;
  }
  char memberName01[] = "CaInfoPath";
  if (settingsDoc->HasMember(memberName01) &&
      (*settingsDoc)[memberName01].IsString()) {
    CaInfoPath = (*settingsDoc)[memberName01].GetString();
  }
  char memberName02[] = "skipPeerVerification";
  if (settingsDoc->HasMember(memberName02) &&
      (*settingsDoc)[memberName02].IsBool()) {
    skipPeerVerification = (*settingsDoc)[memberName02].GetBool();
  }
  char memberName03[] = "skipHostnameVerification";
  if (settingsDoc->HasMember(memberName03) &&
      (*settingsDoc)[memberName03].IsBool()) {
    skipHostnameVerification = (*settingsDoc)[memberName03].GetBool();
  }

  responseJSON_Doc = std::make_shared<rapidjson::Document>();
  fieldsStr = urlPtr->getFieldsStr();

  std::cout << moduleCode
            << "::ClientImpl::open Connection Settings:\nURL Host: "
            << urlPtr->encoded_host() << "\nURL: " << urlPtr->encoded_url()
            << "\nURL Field String: " << fieldsStr
            << "\nHTTP Method: " << method << "\nCaInfoPath: " << CaInfoPath
            << "\nskipPeerVerification: " << skipPeerVerification
            << "\nskipHostnameVerification: " << skipHostnameVerification
            << std::endl;

  curlHandle = curl_easy_init();
  if (!curlHandle) {
    std::cout << "curl_easy_init ERROR" << std::endl;
    jsonReceivedSignal(responseJSON_Doc);
    return -1;
  }

  if (method == "POST") {
    std::cout << moduleCode << "::ClientImpl::open HTTP POST\n";
    curl_easy_setopt(curlHandle, CURLOPT_URL, urlPtr->encoded_url().data());
    curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, fieldsStr.c_str());
  } else {
    curl_easy_setopt(curlHandle, CURLOPT_URL, urlPtr->encoded_url().data());
  }
  curl_easy_setopt(curlHandle, CURLOPT_CAINFO, CaInfoPath.c_str());

  /* handle headers */
  if (headersMapPtr && !headersMapPtr->empty()) {
    struct curl_slist *chunk = NULL;
    for (auto headerIt = headersMapPtr->begin();
         headerIt != headersMapPtr->end(); ++headerIt) {
      std::string headerLine = "";
      headerLine.append(headerIt->first).append("").append(headerIt->second);
      chunk = curl_slist_append(chunk, headerLine.c_str());
    }
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, chunk);
  }

  /*
   * TODO: Write a SSL Certificate Manager
   */
  // getCertificates(global);

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
  if (skipPeerVerification) {
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
  }

  /*
   * If the site you're connecting to uses a different host name that what
   * they have mentioned in their server certificate's commonName (or
   * subjectAltName) fields, libcurl will refuse to connect. You can skip
   * this check, but this will make the connection less secure.
   */
  if (skipHostnameVerification) {
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
  }

  /* setup callbacks */
  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION,
                   bookfiler::curl::writefunc);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &bufferString);
  /* Perform the request, res will get the return code */
  res = curl_easy_perform(curlHandle);

  // Signal data is received
  dataReceivedSignal(bufferString);

  responseJSON_Doc->Parse(bufferString.c_str());

  /* Check for errors */
  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    return -1;
  }

  // Signal JSON received
  jsonReceivedSignal(responseJSON_Doc);

#if HTTPS_GET_JSON_DEBUG
  std::cout << bufferString << std::endl;
#endif

  /* always cleanup */
  curl_easy_cleanup(curlHandle);
  return 0;
}

} // namespace HTTP
} // namespace bookfiler
