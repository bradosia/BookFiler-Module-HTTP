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
| curl/lib/cookie.c		| HTTP cookie parser		|
| ua-parser/uap-cpp		| HTTP User agent parser	|

Boost::Beast does simple HTTP header parsing of each field with name/value pairs. <BR>
Boost::Beast does not do cookie parsing, Forms/File Uploads, or User Agent parsing. <BR>
Unfortunately, there is not a mysql connector that can accompany this list. The official MySQL connector does not compile on mingw.

# Table of Contents

<!--ts-->
   * [Build Instructions](#build-instructions)
   * [Supported](#supported)
   * [Usage Instructions](#usage-instructions)
      * [Creating a server](#creating-a-server)
         * [node.js + hapi.js](#nodejs--hapijs)
         * [C++](#c)
      * [HTTP Routing](#http-routing)
         * [node.js + hapi.js](#nodejs--hapijs-1)
         * [C++](#c-1)
   * [Dependency](#dependency)
   * [License](#license)
<!--te-->

# Build Instructions

## Windows - MinGW-w64
Install MSYS2<BR>
  
```shell
pacman -S mingw-w64-x86_64-fmt mingw-w64-x86_64-boost
```

## Windows - MSVC

```shell

```

## Mac OSX
Only Mac OSX host<BR>
Install clang

## Linux Ubuntu
Install GCC
```shell
sudo apt-get install gcc-multilib
```

## Android
Using Windows x86_64 host for this example:<BR>
Install the android NDK<BR>
Add ```<NDK_PATH>\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin``` to PATH<BR>
Add ```<NDK_PATH>\toolchains\aarch64-linux-android-4.9\prebuilt\windows-x86_64\bin``` to PATH<BR>
You can manually edit the makefile or make with android ndk path like so:
```sh
make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=x86_64 NDK_PATH=C:\android-ndk-r16b-windows-x86_64\android-ndk-r16b
```

## IOS
Only Mac OSX host<BR>
Install clang<BR>
You can manually edit the makefile or make with iphone sdk path like so:
```sh
make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=x86_64 IPHONE_SDK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
```

## Supported
| Host OS | Target OS | Architecture | Support |  Command |
| :---    | :---      | :---         | :---    | :---     |
| Windows | Windows   | x86          | X    | ``` make HOST_OS=WIN TARGET_ARCH=x86 ``` |
| Windows | Windows   | x86_64       | X    | ``` make HOST_OS=WIN TARGET_ARCH=x86_64 ``` |
| Windows | Android   | armeabi      | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=armeabi ``` |
| Windows | Android   | armeabi-v7a  | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=armeabi-v7a ``` |
| Windows | Android   | arm64-v8a    | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=arm64-v8a ``` |
| Windows | Android   | x86          | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=x86 ``` |
| Windows | Android   | x86_64       | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=x86_64 ``` |
| macOS   | macOS     | x86          | O    | ``` make HOST_OS=OSX TARGET_ARCH=x86 ``` |
| macOS   | macOS     | x86_64       | X    | ``` make HOST_OS=OSX TARGET_ARCH=x86_64 ``` |
| macOS   | iOS       | armv7        | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=armv7 ``` |
| macOS   | iOS       | armv7s       | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=armv7s ``` |
| macOS   | iOS       | arm64        | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=arm64 ``` |
| macOS   | iOS       | x86_64       | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=x86_64 ``` |
| linux ubuntu | linux ubuntu | x86     | O    | ``` make HOST_OS=LINUX TARGET_ARCH=x86 ``` |
| linux ubuntu | linux ubuntu | x86_64  | X    | ``` make HOST_OS=LINUX TARGET_ARCH=x86_64 ``` |
 
# Usage Instructions
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
#include <ExpressWeb.h>

auto server = ExpressWeb::Server;
server.connection({ "port", 3000 });
server.start((err) => {
    if (err) {
        throw err;
    }
    console.log(`Server running at: ${server.info.uri}`);
});
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
  httpServer->runAsync();

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
| curl/lib/cookie.c		| MIT-style						| https://github.com/curl/curl/blob/master/lib/cookie.c		|
| ua-parser/uap-cpp		| MIT							| https://github.com/ua-parser/uap-cpp						|

# License
The library is licensed under the MIT License: <BR>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
