cmake_minimum_required(VERSION 3.4)

set(PROJECT_NAME BookFiler-Module-HTTP)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)
set(RELEASE_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/")
set(INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RELEASE_DIR})

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_STATIC_LIBS "Build static library" OFF)
option(BUILD_EXAMPLES "Build example executables" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10")
set(CMAKE_CXX_FLAGS "-g -Og -m64 -Wa,-mbig-obj")
set(CMAKE_BUILD_TYPE "Release")

find_package(Boost 1.56 REQUIRED COMPONENTS
             system filesystem)

include_directories(
  include
  third_party/rapidjson/include
  third_party/boost-url/include
)

link_directories(
)

# Set up source files
set(SOURCES
  src/Module.cpp
  src/core/Curl.cpp
  src/core/httpCurl.cpp
  src/core/httpUtil.cpp
  src/core/httpServer.cpp
  src/core/httpServerListener.cpp
  src/core/httpServerSession.cpp
  src/core/httpServerUtil.cpp
  src/core/httpServerCertificate.cpp
  src/core/json.cpp
  src/core/certificateManager.hpp
)

set(HEADERS
  src/Module.hpp
  src/core/config.hpp
  src/core/Curl.hpp
  src/core/httpCurl.hpp
  src/core/httpUtil.hpp
  src/core/httpServer.hpp
  src/core/httpServerListener.hpp
  src/core/httpServerSession.hpp
  src/core/httpServerUtil.hpp
  src/core/httpServerCertificate.hpp
  src/core/json.hpp
  src/core/certificateManager.cpp

  include/BookFiler-Module-HTTP-Curl/Interface.hpp
)

set(SHARED_COMPILE_DEFINITIONS
  -DSHARED_LIBRARY
)

set(STATIC_COMPILE_DEFINITIONS
  -DSTATIC_LIBRARY
)

set(LIBRARIES ${LIBRARIES}
  # Boost
  Boost::system
  Boost::filesystem

  # curl
  curl
  nghttp2
  psl.dll
  idn2
  cares
  ssh2
  brotlidec-static
  brotlienc-static
  brotlicommon-static
  unistring
  wldap32
  bcrypt

  # ssl
  ssl
  crypto
  crypt32

  # zlib
  z
  libz
)

if(WIN32)
    set(LIBRARIES ${LIBRARIES}
        # Windows Libraries
        ws2_32
        secur32
        wsock32
        shlwapi
    )
elseif(UNIX)
    set(LIBRARIES ${LIBRARIES}
        # Unix Libraries
    )
endif()

set(SHARED_LINK_LIBRARIES ${LIBRARIES})
set(STATIC_LINK_LIBRARIES ${LIBRARIES})

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(CMakePackageConfigHelpers)

if(BUILD_SHARED_LIBS)
  set(lib_shared_name ${PROJECT_NAME}-LibShared)
  message( STATUS "-- Building ${lib_shared_name}" )
  add_library(${lib_shared_name} SHARED ${SOURCES} ${HEADERS})
  set_property(TARGET ${lib_shared_name} PROPERTY VERSION "${PROJECT_VERSION}")
  target_include_directories(${lib_shared_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  #set_target_properties(${lib_shared_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")

  # Compile Definitions
  target_compile_definitions(${lib_shared_name} PUBLIC ${SHARED_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_shared_name} PUBLIC ${SHARED_LINK_LIBRARIES})
endif()

if(BUILD_STATIC_LIBS)
  set(lib_name ${PROJECT_NAME}-Lib)
  message( STATUS "-- Building ${lib_name}" )
  add_library(${lib_name} STATIC ${SOURCES} ${HEADERS})
  set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
  target_include_directories(${lib_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  # Compile Definitions
  target_compile_definitions(${lib_name} PUBLIC ${STATIC_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_name} PUBLIC ${STATIC_LINK_LIBRARIES})
endif()

# EXAMPLES
add_subdirectory(src_example/http-connect)
add_subdirectory(src_example/oauth2-google)

# Post build
if(BUILD_SHARED_LIBS)
  add_custom_command(TARGET ${lib_shared_name} POST_BUILD
    # Make a release directory without all the cmake extras.
    # This directory can easily be archived and distributed.
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}"
    # Create release sub-folders
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}modules/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}resources/"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_shared_name}> "${RELEASE_DIR}modules/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources/" "${RELEASE_DIR}resources"
    COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/settings/settings.json" "${RELEASE_DIR}settings.json"
  )
endif(BUILD_SHARED_LIBS)

# Installation
if(BUILD_SHARED_LIBS)
  message(STATUS "INSTALL: ${lib_shared_name} TO root=${CMAKE_BINARY_DIR} and current=${CMAKE_CURRENT_BINARY_DIR}")
  install(TARGETS ${lib_shared_name}
    RUNTIME DESTINATION "${INSTALL_DIR}/modules"
    ARCHIVE DESTINATION "${INSTALL_DIR}/modules"
    LIBRARY DESTINATION "${INSTALL_DIR}/modules"
  )
endif(BUILD_SHARED_LIBS)

if(BUILD_STATIC_LIBS)
  install(TARGETS ${lib_name} EXPORT ${lib_name}_TARGETS
    RUNTIME DESTINATION "${INSTALL_DIR}/lib"
    ARCHIVE DESTINATION "${INSTALL_DIR}/lib"
    LIBRARY DESTINATION "${INSTALL_DIR}/lib"
  )
endif(BUILD_STATIC_LIBS)

install(FILES ${HEADERS}
  DESTINATION "${INSTALL_DIR}/headers"
COMPONENT Headers)
