# Usage Instructions
This module uses an API similar to that of node.js and select parts taken from hapi.js and express.js. The node.js code will be compared to the equivalent BookFiler-Module-HTTP C++ Code to see how the two API compare.
## Sending Cookies

### node.js + express.js
```node
 res.cookie('userId', userId,  
 { maxAge: 1000,
   path : '/',
   // You can't access these tokens in the client's javascript
   httpOnly: true,
   // Forces to use https in production
   secure: process.env.NODE_ENV === 'production'? true: false
 });
```

### c++
```cpp
httpServer->route({{"method", "GET"},
                     {"path", "/"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                         res.setCookie("name", "value",  
                          { {"maxAge", 1000},
                            {"path", "/"},
                          });
                         return "Set Cookie";
                      }}});
```

## Getting Cookies

### node.js + express.js
```node
req.cookies['name'];
```

### c++
```cpp
httpServer->route({{"method", "GET"},
                     {"path", "/"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                        req.getCookie("name").value();
                        return "Get Cookie";
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
