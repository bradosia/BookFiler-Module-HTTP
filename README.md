# BookFiler HTTP Module
A hand-picked HTTP and WebSocket toolkit for C++. This criteria for the selection was compatibility with mingw, GCC, and clang and free use for commercial applications. This module is a glue between established libraries. This module tries to integrate other projects rather than reinventing the wheel.

This module is meant to be used with the module system for BookFiler software suite of programs. The module system is designed to separately compile modules, so that the end application doesnt require a large compilation and linking step. The modules are similar to dynamic linked libraries, except that they don't need to be linked to the main application. This way if a module is missing, the main application can give a more detailed and graphically friendly message indicating missing modules rather than an ugly operating system error window.

## Dependencies

| Library				| Purpose					|
| :---					| :---						|
| Boost::Asio			| Networking 				|
| Boost::Beast			| HTTP and WebSocket		|
| Boost::Filesystem		| Filesystem				|
| Boost::url		| URL				|
| Boost::nowide			| utf8 support				|
| rapidJSON				| JSON parser				|
| kainjow::mustache				| templating and views		|
| openssl				| SSL						|
| ua-parser/uap-cpp		| HTTP User agent parser	|
| yaml-cpp | A dependency of ua-parser/uap-cpp |

Boost::Beast does simple HTTP header parsing of each field with name/value pairs.<BR>
Boost::Beast does not do cookie parsing, Forms/File Uploads, or User Agent parsing.<BR>
This module extends Boost::Beast by adding in cookie parsing, Forms/File Uploads, and User Agent parsing.

# Table of Contents

<!--ts-->
   * [Build Instructions](#build-instructions)
   * [Usage Instructions](#usage-instructions)
      * [Creating a server](#creating-a-server)
         * [node.js + hapi.js](#nodejs--hapijs)
         * [C++](#c)
      * [HTTP Routing](#http-routing)
         * [node.js + hapi.js](#nodejs--hapijs-1)
         * [C++](#c-1)
      * [Cookies](/docs/cookies.md)
   * [Dependency](#dependency)
   * [License](#license)
<!--te-->

# Build Instructions

## Windows - MinGW-w64
Install MSYS2<BR>
Then, install GCC, cmake, git and dependencies. Confirmed working with Windows 10 as of 11/8/2020.
```shell
pacman -S mingw-w64-x86_64-gcc cmake git
pacman -S mingw-w64-x86_64-boost mingw-w64-x86_64-openssl mingw-w64-x86_64-re2 mingw-w64-x86_64-zlib mingw-w64-x86_64-fmt
```
Build:
```shell
git clone https://github.com/bradosia/BookFiler-Module-HTTP
cd BookFiler-Module-HTTP
mkdir build
cd build
cmake -G "MSYS Makefiles" ../
make
```

## Linux Ubuntu
Install GCC, cmake, git and dependencies. Confirmed working with Ubuntu 20.04 as of 11/8/2020.
```shell
sudo apt-get update
sudo apt install build-essential gcc-multilib g++-multilib cmake git
sudo apt install libboost-all-dev libssl-dev libre2-dev zlib1g libfmt-dev
```
Build:
```shell
git clone https://github.com/bradosia/BookFiler-Module-HTTP
cd BookFiler-Module-HTTP
mkdir build
cd build
cmake ../
make
```
 
# Usage Instructions

This module uses an API similar to that of node.js and select parts taken from hapi.js and express.js. The node.js code will be compared to the equivalent BookFiler-Module-HTTP C++ Code to see how the two API compare.
## Creating a server

### node.js + hapi.js
```node
const Hapi = require('hapi');
const server = new Hapi.Server();
server.connection({ port: 3000 });
server.start((err) => {
    if (err) {
        throw err;
    }
    console.log(`Server running at: ${server.info.uri}`);
});
```

### c++
```cpp
httpServer = httpModule->newServer({{"port", 3000}, {"host", "localhost"}});
httpServer->run();
```

## HTTP Routing

### node.js + hapi.js
```node
const Hapi = require('hapi');

const server = Hapi.server({
    port: 3000,
    host: 'localhost'
});

server.route({
    method: 'GET',
    path: '/',
    handler: (request, h) => {

        return 'Hello, world!';
    }
});

server.route({
    method: 'GET',
    path: '/{name}',
    handler: (request, h) => {

        return 'Hello, ' + encodeURIComponent(request.params.name) + '!';
    }
});

const init = async () => {

    await server.start();
    console.log(`Server running at: ${server.info.uri}`);
};

process.on('unhandledRejection', (err) => {

    console.log(err);
    process.exit(1);
});

init();
```

### C++
```cpp
#include <BookFilerModuleHttpLoader.hpp>

int allModulesLoaded();

std::shared_ptr<bookfiler::HTTP::ModuleInterface> httpModule;
std::shared_ptr<bookfiler::HTTP::Server> httpServer;

int main() {
  bookfiler::HTTP::loadModule("modules", std::bind(&allModulesLoaded),
                              httpModule);

  system("pause");
  return 0;
}

int allModulesLoaded() {
  // SSL Certificate manager
  std::shared_ptr<bookfiler::certificate::Manager> certificateManager =
      httpModule->newCertificateManager();
  std::shared_ptr<bookfiler::certificate::Certificate> certRootPtr,
      certServerPtr;
  certificateManager->loadCertificate(certRootPtr);

  // Setup server
  httpServer = httpModule->newServer({{"port", 3000}, {"host", "localhost"}});
  httpServer->useCertificate(certRootPtr);

  // Route by using a lambda expression
  httpServer->route({{"method", "GET"},
                     {"path", "/"},
                     {"handler",
                      [](bookfiler::HTTP::request req,
                         bookfiler::HTTP::response res) -> std::string {
                        return "Hello World!";
                      }}});

  // Start server
  httpServer->run();

  return 0;
}
```


## Dependency

| Library				| License						| Link														|
| :---					| :---							| :---														|
| Boost::Asio			| Boost Software License 1.0 	| https://github.com/boostorg/asio 							|
| Boost::Beast			| Boost Software License 1.0	| https://github.com/boostorg/beast							|
| Boost::Filesystem		| Boost Software License 1.0	| 															|
| Boost::nowide			| Boost Software License 1.0	| https://github.com/artyom-beilis/nowide					|								|
| rapidJSON				| MIT							| https://github.com/Tencent/rapidjson						|
| bustache				| Boost Software License 1.0	| https://github.com/jamboree/bustache						|
| openssl				| attribute						| https://github.com/openssl/openssl						|
| ua-parser/uap-cpp		| MIT							| https://github.com/ua-parser/uap-cpp						|

# License
The library is licensed under the MIT License: <BR>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
