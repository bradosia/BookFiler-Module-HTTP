/*
 * @name BookFiler Module - HTTP w/ Curl
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

#ifndef BOOKFILER_MODULE_HTTP_HTTP_SERVER_CERTIFICATE_H
#define BOOKFILER_MODULE_HTTP_HTTP_SERVER_CERTIFICATE_H

// C++17
//#include <filesystem>
#include <algorithm>
#include <cstddef>
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
#include <boost/asio/buffer.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/signals2.hpp>

// Local Project
#include <BookFiler-Module-HTTP/Interface.hpp>

/*
 * bookfiler - HTTP
 */
namespace bookfiler {
namespace HTTP {

/*  Load a signed certificate into the ssl context, and configure
    the context for use with a server.
    For this to work with the browser or operating system, it is
    necessary to import the "Beast Test CA" certificate into
    the local certificate store, browser, or operating system
    depending on your environment Please see the documentation
    accompanying the Beast certificate for more details.
*/
inline void load_server_certificate(boost::asio::ssl::context &ctx) {
  /*
      The certificate was generated from CMD.EXE on Windows 10 using:
      winpty openssl dhparam -out dh.pem 2048
      winpty openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days
     10000 -out cert.pem -subj "//C=US\ST=CA\L=Los
     Angeles\O=Beast\CN=www.example.com"
  */

  std::string const cert =
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDEDCCAfigAwIBAwICBAAwDQYJKoZIhvcNAQELBQAwSzELMAkGA1UEBhMCVVMx\n"
      "EjAQBgNVBAoMCUJvb2tGaWxlcjEoMCYGA1UEAwwfQm9va0ZpbGVyIENlcnRpZmlj\n"
      "YXRlIEF1dGhvcml0eTAeFw0yMDEwMTgwOTI4NTFaFw0yNjA0MTAwOTI4NTFaMEsx\n"
      "CzAJBgNVBAYTAlVTMRIwEAYDVQQKDAlCb29rRmlsZXIxKDAmBgNVBAMMH0Jvb2tG\n"
      "aWxlciBMb2NhbGhvc3QgQ2VydGlmaWNhdGUwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n"
      "DwAwggEKAoIBAQDqJKAP7CDA5/lv4Lu+m/8X9qt05OBc0x+TJdJ5CAxnruPnnkWQ\n"
      "KBqDfdj1Zcz/TFETFzh8se2VTpQWJQSsu4TkYD3818lfdpxulznXlldgPGOiqrIS\n"
      "MnYwJe+3DzzTO1LfjGTEnxZWT80BXp74M9rXmgtyuXOiv0vvgKBH61UjgQtgFm4o\n"
      "kLL4NOrm2WLoveYJP96SN/vpwp9IafRxNt7XFgTvtYD8cPjAsYBDvz612Rgm7PYI\n"
      "PBe3nxA5dStlsLc3E14WH7vNdrRdWQpmKniBwSndpTwnsq+X0Ccue+oFWYAUFgdG\n"
      "J29+vhRnpkc0I+pvSNPMj8mKxoprtK7D05HnAgMBAAEwDQYJKoZIhvcNAQELBQAD\n"
      "ggEBAJ5pfaINNn2e20MWQml0j6x1bLAUQ41njabzrmM2S8TWnRjlJ7YfgXulL54N\n"
      "ZaGunhBG4LKEpsuDnNLDUpgnmYkuZP5h5ihZ01NHRzazUd4IZoJjZsV9KIVxNaxy\n"
      "REmL7zsAGPT2WsHu4HyEEC93LlP0b5c1SxM+0Hma/GfaFt+7oXk1crumhVG8Yyw1\n"
      "lvGCcO5Ak8Ple0aNwoUl1smfFUlNbqjuBQG9uPluVm42iOJHn+3NCJhP0TQmkYJV\n"
      "heNREmCI5hQx0IWa7XxvcquinguowWgQ0rg/uGrAl4yfLn5I+QufqTXXxRkt53Ju\n"
      "QRCxdTdPl7vXM//M4+8rUdWbTDk=\n"
      "-----END CERTIFICATE-----\n";

  std::string const key =
      "-----BEGIN PRIVATE KEY-----\n"
      "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDAc3aOFObf66Ah\n"
      "hJyrnh/a71OGA5ey3Mff5fqvVF/5KZRxBz1oIf6TbNCQH9JA2uyks+6+zd+Z/cTH\n"
      "OSZnLaudw8PgDqe1TYNs2jyqGEFaZOj3Pq0XroNDpKAneWAUJzP/5n45hckJ/ISA\n"
      "rWQdcmPneLUD6iAbbQ0lSCllxKo8cQIYiLZO5U1+vs7ab7liVy4rL+2H4nJISOZ2\n"
      "hI8eY2kIHdigTYE5L7SkumNxBxNuaJeX0dpiOpeghHpIwn2FXtxXyypnai5lMzkX\n"
      "7Om6s9lzEQYjAvUZ2OoU62q6u3oD81DwnCKVdFzlHgqyORcONA2yNSS/dV0xW2bl\n"
      "tNRljr0tAgMBAAECggEAfkSDeY8/N5WKN9OhU3xo3CWAQIJk6qauwTlMM/MBESJ1\n"
      "0X71jDgZprKTWKx4L7Mz5pBjyW6cEZYNoMTfbHzrh5RlFMGoyWy1GRtLCDLn1h41\n"
      "N9s4iDwoxZ3ismORZUefd3vMP4OGtvWmHiMld6iQ4dMxuQLI1BXFtTtWZf4Vz5cW\n"
      "jAi4qCK1ULmT4Kdn4IZ6Vj9rsp81njIZluPQRcIpLnImhJTcJUL/rIF/P55XLiIA\n"
      "Uf0/fxW8cqvmXXsNTBZ6nFEJ1ueGXUyj+Dy7GHWlbbQE973aOA9fpHezGBN5BMf4\n"
      "obshYYNbipGLyEntEZxf7fx4hexC60JbmkoP/X8+oQKBgQDeJrRIOkiT4cABPmzi\n"
      "BIg9Ph5VnCV5Po/k1MCwzf76hcbFDDYopCBzKA2ZJhcp9v8RKkLlOdpyirZpFNOg\n"
      "fTelOPZxkte3MrYzf0+UGWcR85Z5qSqmLYzGHY+uowZjAEc+VoFS75aDkCvqPFGm\n"
      "nOVjsLL/QzYqlKQN0xPoOpraFQKBgQDdxkP5GTrkcnB++Pgmvx5HrAnoWqYafrUJ\n"
      "XZ+amZuAygONoqhQAJrkfosbJfMgzXcAXT+A7E7bumWXvZuy0d6Nl5WhZza397/2\n"
      "fJAY/Dy2C7nCpkEOf9YwYu6w0t/G3v3uvTVvYUCqu7RzepprxZKAoEPyI5AbwlFS\n"
      "Sy8hEweUuQKBgQDTH91+N0X6pTWg+P7p3nzMF1kk630BuV9vjnKClYOZ+8FoS668\n"
      "+R6N0qb5waWKEF9gpNVmnJDrAsFRJKXrhJV5xKUSUwgH534ZewjaKzo1aVa80PTj\n"
      "ca60Kiv/QJg1RnJu4C+P7nsRFddmiWTf/4RSNgAncGzuEs3HBa+JqQi58QKBgQCw\n"
      "nnKJ0mcNgnpHFL6EFRbnW4K0cmTwmtJQuCggto6aPcggp1bGCAVTTdg48bsz9437\n"
      "U0f8w7V11neMJiv2eJ92Qi+fUBTK5FsT0VXtIAy1qpbxBviutG5/MHsJhZdDyVmQ\n"
      "JU2F+3zFvCFVCmIqzDkFiR0znNDq2pFrKYdKHt6duQKBgEMqLsVeBs/uRFlxiIHH\n"
      "ptDLORNIAOxz/+rkY6zKML205M3Y4S6hZRUeaCiS6p3J5ZZh7Aq+WGfGIUXJnNKT\n"
      "BQpR9lX7So+iTnbNh6F4kJMnOpdxmWbybdh6IXxB90f34in8gKrc5Kmdz2tEg8Ui\n"
      "Xo7EbeA/uHZoDSlsoVtF96pQ\n"
      "-----END PRIVATE KEY-----\n";

  std::string const dh =
      "-----BEGIN DH PARAMETERS-----\n"
      "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwHN2jhTm3+ugIYScq54f\n"
      "2u9ThgOXstzH3+X6r1Rf+SmUcQc9aCH+k2zQkB/SQNrspLPuvs3fmf3ExzkmZy2r\n"
      "ncPD4A6ntU2DbNo8qhhBWmTo9z6tF66DQ6SgJ3lgFCcz/+Z+OYXJCfyEgK1kHXJj\n"
      "53i1A+ogG20NJUgpZcSqPHECGIi2TuVNfr7O2m+5YlcuKy/th+JySEjmdoSPHmNp\n"
      "CB3YoE2BOS+0pLpjcQcTbmiXl9HaYjqXoIR6SMJ9hV7cV8sqZ2ouZTM5F+zpurPZ\n"
      "cxEGIwL1GdjqFOtqurt6A/NQ8JwilXRc5R4KsjkXDjQNsjUkv3VdMVtm5bTUZY69\n"
      "LQIDAQAB\n"
      "-----END DH PARAMETERS-----\n";

  ctx.set_password_callback(
      [](std::size_t, boost::asio::ssl::context_base::password_purpose) {
        return "test";
      });

  ctx.set_options(boost::asio::ssl::context::default_workarounds |
                  boost::asio::ssl::context::no_sslv2 |
                  boost::asio::ssl::context::single_dh_use);

  ctx.use_certificate_chain(boost::asio::buffer(cert.data(), cert.size()));

  ctx.use_private_key(boost::asio::buffer(key.data(), key.size()),
                      boost::asio::ssl::context::file_format::pem);

  ctx.use_tmp_dh(boost::asio::buffer(dh.data(), dh.size()));
}

/*  Load a signed certificate into the ssl context, and configure
    the context for use with a server.
    For this to work with the browser or operating system, it is
    necessary to import the "Beast Test CA" certificate into
    the local certificate store, browser, or operating system
    depending on your environment Please see the documentation
    accompanying the Beast certificate for more details.
*/
inline void
loadServerCertificate(std::shared_ptr<boost::asio::ssl::context> sslContext) {
  /*
      The certificate was generated from CMD.EXE on Windows 10 using:
      winpty openssl dhparam -out dh.pem 2048
      winpty openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days
     10000 -out cert.pem -subj "//C=US\ST=CA\L=Los
     Angeles\O=Beast\CN=www.example.com"
  */

  std::string const cert =
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDaDCCAlCgAwIBAgIJAO8vBu8i8exWMA0GCSqGSIb3DQEBCwUAMEkxCzAJBgNV\n"
      "BAYTAlVTMQswCQYDVQQIDAJDQTEtMCsGA1UEBwwkTG9zIEFuZ2VsZXNPPUJlYXN0\n"
      "Q049d3d3LmV4YW1wbGUuY29tMB4XDTE3MDUwMzE4MzkxMloXDTQ0MDkxODE4Mzkx\n"
      "MlowSTELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAkNBMS0wKwYDVQQHDCRMb3MgQW5n\n"
      "ZWxlc089QmVhc3RDTj13d3cuZXhhbXBsZS5jb20wggEiMA0GCSqGSIb3DQEBAQUA\n"
      "A4IBDwAwggEKAoIBAQDJ7BRKFO8fqmsEXw8v9YOVXyrQVsVbjSSGEs4Vzs4cJgcF\n"
      "xqGitbnLIrOgiJpRAPLy5MNcAXE1strVGfdEf7xMYSZ/4wOrxUyVw/Ltgsft8m7b\n"
      "Fu8TsCzO6XrxpnVtWk506YZ7ToTa5UjHfBi2+pWTxbpN12UhiZNUcrRsqTFW+6fO\n"
      "9d7xm5wlaZG8cMdg0cO1bhkz45JSl3wWKIES7t3EfKePZbNlQ5hPy7Pd5JTmdGBp\n"
      "yY8anC8u4LPbmgW0/U31PH0rRVfGcBbZsAoQw5Tc5dnb6N2GEIbq3ehSfdDHGnrv\n"
      "enu2tOK9Qx6GEzXh3sekZkxcgh+NlIxCNxu//Dk9AgMBAAGjUzBRMB0GA1UdDgQW\n"
      "BBTZh0N9Ne1OD7GBGJYz4PNESHuXezAfBgNVHSMEGDAWgBTZh0N9Ne1OD7GBGJYz\n"
      "4PNESHuXezAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCmTJVT\n"
      "LH5Cru1vXtzb3N9dyolcVH82xFVwPewArchgq+CEkajOU9bnzCqvhM4CryBb4cUs\n"
      "gqXWp85hAh55uBOqXb2yyESEleMCJEiVTwm/m26FdONvEGptsiCmF5Gxi0YRtn8N\n"
      "V+KhrQaAyLrLdPYI7TrwAOisq2I1cD0mt+xgwuv/654Rl3IhOMx+fKWKJ9qLAiaE\n"
      "fQyshjlPP9mYVxWOxqctUdQ8UnsUKKGEUcVrA08i1OAnVKlPFjKBvk+r7jpsTPcr\n"
      "9pWXTO9JrYMML7d+XRSZA1n3856OqZDX4403+9FnXCvfcLZLLKTBvwwFgEFGpzjK\n"
      "UEVbkhd5qstF6qWK\n"
      "-----END CERTIFICATE-----\n";

  std::string const key =
      "-----BEGIN PRIVATE KEY-----\n"
      "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDJ7BRKFO8fqmsE\n"
      "Xw8v9YOVXyrQVsVbjSSGEs4Vzs4cJgcFxqGitbnLIrOgiJpRAPLy5MNcAXE1strV\n"
      "GfdEf7xMYSZ/4wOrxUyVw/Ltgsft8m7bFu8TsCzO6XrxpnVtWk506YZ7ToTa5UjH\n"
      "fBi2+pWTxbpN12UhiZNUcrRsqTFW+6fO9d7xm5wlaZG8cMdg0cO1bhkz45JSl3wW\n"
      "KIES7t3EfKePZbNlQ5hPy7Pd5JTmdGBpyY8anC8u4LPbmgW0/U31PH0rRVfGcBbZ\n"
      "sAoQw5Tc5dnb6N2GEIbq3ehSfdDHGnrvenu2tOK9Qx6GEzXh3sekZkxcgh+NlIxC\n"
      "Nxu//Dk9AgMBAAECggEBAK1gV8uETg4SdfE67f9v/5uyK0DYQH1ro4C7hNiUycTB\n"
      "oiYDd6YOA4m4MiQVJuuGtRR5+IR3eI1zFRMFSJs4UqYChNwqQGys7CVsKpplQOW+\n"
      "1BCqkH2HN/Ix5662Dv3mHJemLCKUON77IJKoq0/xuZ04mc9csykox6grFWB3pjXY\n"
      "OEn9U8pt5KNldWfpfAZ7xu9WfyvthGXlhfwKEetOuHfAQv7FF6s25UIEU6Hmnwp9\n"
      "VmYp2twfMGdztz/gfFjKOGxf92RG+FMSkyAPq/vhyB7oQWxa+vdBn6BSdsfn27Qs\n"
      "bTvXrGe4FYcbuw4WkAKTljZX7TUegkXiwFoSps0jegECgYEA7o5AcRTZVUmmSs8W\n"
      "PUHn89UEuDAMFVk7grG1bg8exLQSpugCykcqXt1WNrqB7x6nB+dbVANWNhSmhgCg\n"
      "VrV941vbx8ketqZ9YInSbGPWIU/tss3r8Yx2Ct3mQpvpGC6iGHzEc/NHJP8Efvh/\n"
      "CcUWmLjLGJYYeP5oNu5cncC3fXUCgYEA2LANATm0A6sFVGe3sSLO9un1brA4zlZE\n"
      "Hjd3KOZnMPt73B426qUOcw5B2wIS8GJsUES0P94pKg83oyzmoUV9vJpJLjHA4qmL\n"
      "CDAd6CjAmE5ea4dFdZwDDS8F9FntJMdPQJA9vq+JaeS+k7ds3+7oiNe+RUIHR1Sz\n"
      "VEAKh3Xw66kCgYB7KO/2Mchesu5qku2tZJhHF4QfP5cNcos511uO3bmJ3ln+16uR\n"
      "GRqz7Vu0V6f7dvzPJM/O2QYqV5D9f9dHzN2YgvU9+QSlUeFK9PyxPv3vJt/WP1//\n"
      "zf+nbpaRbwLxnCnNsKSQJFpnrE166/pSZfFbmZQpNlyeIuJU8czZGQTifQKBgHXe\n"
      "/pQGEZhVNab+bHwdFTxXdDzr+1qyrodJYLaM7uFES9InVXQ6qSuJO+WosSi2QXlA\n"
      "hlSfwwCwGnHXAPYFWSp5Owm34tbpp0mi8wHQ+UNgjhgsE2qwnTBUvgZ3zHpPORtD\n"
      "23KZBkTmO40bIEyIJ1IZGdWO32q79nkEBTY+v/lRAoGBAI1rbouFYPBrTYQ9kcjt\n"
      "1yfu4JF5MvO9JrHQ9tOwkqDmNCWx9xWXbgydsn/eFtuUMULWsG3lNjfst/Esb8ch\n"
      "k5cZd6pdJZa4/vhEwrYYSuEjMCnRb0lUsm7TsHxQrUd6Fi/mUuFU/haC0o0chLq7\n"
      "pVOUFq5mW8p0zbtfHbjkgxyF\n"
      "-----END PRIVATE KEY-----\n";

  std::string const dh =
      "-----BEGIN DH PARAMETERS-----\n"
      "MIIBCAKCAQEArzQc5mpm0Fs8yahDeySj31JZlwEphUdZ9StM2D8+Fo7TMduGtSi+\n"
      "/HRWVwHcTFAgrxVdm+dl474mOUqqaz4MpzIb6+6OVfWHbQJmXPepZKyu4LgUPvY/\n"
      "4q3/iDMjIS0fLOu/bLuObwU5ccZmDgfhmz1GanRlTQOiYRty3FiOATWZBRh6uv4u\n"
      "tff4A9Bm3V9tLx9S6djq31w31Gl7OQhryodW28kc16t9TvO1BzcV3HjRPwpe701X\n"
      "oEEZdnZWANkkpR/m/pfgdmGPU66S2sXMHgsliViQWpDCYeehrvFRHEdR9NV+XJfC\n"
      "QMUk26jPTIVTLfXmmwU0u8vUkpR7LQKkwwIBAg==\n"
      "-----END DH PARAMETERS-----\n";

  sslContext->set_password_callback(
      [](std::size_t, boost::asio::ssl::context_base::password_purpose) {
        return "test";
      });

  sslContext->set_options(boost::asio::ssl::context::default_workarounds |
                          boost::asio::ssl::context::no_sslv2 |
                          boost::asio::ssl::context::single_dh_use);

  sslContext->use_certificate_chain(
      boost::asio::buffer(cert.data(), cert.size()));

  sslContext->use_private_key(boost::asio::buffer(key.data(), key.size()),
                              boost::asio::ssl::context::file_format::pem);

  sslContext->use_tmp_dh(boost::asio::buffer(dh.data(), dh.size()));
}

} // namespace HTTP
} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_MYSQL_HTTP_H
