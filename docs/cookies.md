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
Cookie public interface is defined in [/include/BookFiler-Module-HTTP/Interface.hpp](/include/BookFiler-Module-HTTP/Interface.hpp) and the private header is in [/src/core/RequestCookie.hpp](/src/core/RequestCookie.hpp). The definition is in [/src/core/RequestCookie.cpp](/src/core/RequestCookie.cpp)

# Example

[/src_example/http-server-cookie/main.cpp](/src_example/http-server-cookie/main.cpp)

# Credits

The cookie parser was heavily inspired by POCO C++ Libraries: 
https://pocoproject.org/docs/Poco.Net.HTTPCookie.html
