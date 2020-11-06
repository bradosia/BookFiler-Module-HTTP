set(EXENAME http-server-minimal)

set(SOURCES_SUB ${SOURCES_SUB}
  main.cpp
)

set(HEADERS_SUB
  src/Module.hpp
  src/core/config.hpp

  BookFiler-Module-HTTP/Interface.hpp
)

include_directories(
  ../../include
  ../../third_party/rapidjson/include
  ../../third_party/Module-Manager/include
  ../../third_party/Settings-Manager/include
)

link_directories(
  ../../third_party/Module-Manager/lib
  ../../third_party/Settings-Manager/lib
)

add_executable(${EXENAME} ${SOURCES_SUB})

target_link_libraries(${EXENAME} PRIVATE
    # Boost
    Boost::system
    Boost::filesystem

    ModuleManager
    SettingsManager
)
