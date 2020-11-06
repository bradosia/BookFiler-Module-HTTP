# Usage Instructions
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
bookfiler::HTTP::Cookie = req.getCookie("name");
```
Cookie interface is defined in [/include/BookFiler-Module-HTTP/Interface.hpp](/include/BookFiler-Module-HTTP/Interface.hpp) and implemented in [/src/core/httpCookie.hpp](/src/core/httpCookie.hpp).
