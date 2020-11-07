# Usage Instructions
This module uses an API similar to that of node.js and select parts taken from hapi.js and express.js. The node.js code will be compared to the equivalent BookFiler-Module-HTTP C++ Code to see how the two API compare.
## Sending Cookies

### node.js + express.js
```node
app.get('/get', function (req, res) {
 res.cookie('name', 'value',  
 { maxAge: 1000,
   path : '/'
 });
})
```

### c++
```cpp
httpServer->route(
      {{"method", "GET"},
       {"path", "*"},
       {"handler",
        [](bookfiler::HTTP::request req,
           bookfiler::HTTP::response res) -> std::string {
          // Set cookie
          res->setCookie("name", "value", {});
          return "<h1>Set cookie</h1><br><a href=\"/get\">Get Cookie</a>";
        }}});
```

## Getting Cookies

### node.js + express.js
```node
app.get('/get', function (req, res) {
  return "Get cookie name=" + req.cookies["name"].value;
})
```

### c++
```cpp
httpServer->route({{"method", "GET"},
                     {"path", "/get"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                        // Get cookie
                        std::string_view value =
                            req->getCookie("name")->value();
                        std::string bodyStr;
                        bodyStr.append("Get cookie name=").append(value);
                        return bodyStr;
                      }}});
```

# Implementation

```cpp
// Get a cookie object by cookie name
bookfiler::HTTP::Cookie = req.getCookie("name");
```
Cookie public interface is defined in [/include/BookFiler-Module-HTTP/Interface.hpp](/include/BookFiler-Module-HTTP/Interface.hpp). The public interface is the outward facing interface for using the module. ACookie is an object that represents a single cookie..

* The cookie private header [/src/core/RequestCookie.hpp](/src/core/RequestCookie.hpp)
* The cookie implementation [/src/core/RequestCookie.hpp](/src/core/RequestCookie.hpp)

The Cookie Map is a C++ Map that contains multiple cookies. 
* The cookie map private header [/src/core/RequestCookieMap.hpp](/src/core/RequestCookieMap.hpp)
* The cookie map implementation [/src/core/RequestCookieMap.hpp](/src/core/RequestCookieMap.hpp)

Each `bookfiler::http::RequestImpl` contains a `http::CookieMap`. When `http::RequestImpl` `parseRequest()` is called, the `boost::beast::http::request` will have its cookie header parsed into the `http::RequestImpl` `http::CookieMap`.

Each `bookfiler::http::ResponseImpl` contains a `http::CookieMap`. When `http::ResponseImpl` `parseResponse()` is called, the `http::ResponseImpl` `http::CookieMap` will have its cookie data combined into the `boost::beast::http::response`.

# Example

[/src_example/http-server-cookie/main.cpp](/src_example/http-server-cookie/main.cpp)

# Credits

The cookie parser was heavily inspired by POCO C++ Libraries: 
https://pocoproject.org/docs/Poco.Net.HTTPCookie.html
