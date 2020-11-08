# BookFiler HTTP Module
A hand-picked HTTP and WebSocket toolkit for C++. This criteria for the selection was compatibility with MSVC, mingw, GCC, and clang and free use for commercial applications. This module is a glue between established libraries. This module tries to integrate other projects rather than reinventing the wheel.

## Dependencies

| Library				| Purpose					|
| :---					| :---						|
| Boost::Asio			| Networking 				|
| Boost::Beast			| HTTP and WebSocket		|
| Boost::Filesystem		| Filesystem				|
| Boost::url		| URL				|
| Boost::nowide			| utf8 support				|
| rapidJSON				| JSON parser				|
| bustache				| templating and views		|
| openssl				| SSL						|
| ua-parser/uap-cpp		| HTTP User agent parser	|

Boost::Beast does simple HTTP header parsing of each field with name/value pairs. <BR>
Boost::Beast does not do cookie parsing, Forms/File Uploads, or User Agent parsing. <BR>

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
Install Dependencies:
```shell
pacman -S mingw-w64-x86_64-fmt mingw-w64-x86_64-boost mingw-w64-x86_64-re2
```
Build:
```shell
mkdir build
cd build
cmake -G "MSYS Makefiles" ../
```
  
## Windows - MSVC
```shell
```

## Linux Ubuntu
Install GCC-10, cmake, git and dependencies. Confirmed working with Ubuntu 20.04.
```shell
sudo apt-get update
sudo apt install build-essential gcc-multilib cmake git g++-10 gcc-10 g++-10-multilib
// Use g++ 10
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 20
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20
sudo apt install libboost-all-dev libssl-dev libre2-dev zlib1g libfmt-dev
```
Build:
```shell
mkdir build
cd build
cmake ../
make
```

## Mac OSX
Only Mac OSX host<BR>
Install clang

## Android
Using Windows x86_64 host for this example:<BR>
Install the android NDK<BR>
Add ```<NDK_PATH>\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin``` to PATH<BR>
Add ```<NDK_PATH>\toolchains\aarch64-linux-android-4.9\prebuilt\windows-x86_64\bin``` to PATH<BR>
You can manually edit the makefile or make with android ndk path like so:
```shell
```

## IOS
Only Mac OSX host<BR>
Install clang<BR>
You can manually edit the makefile or make with iphone sdk path like so:
```shell
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
