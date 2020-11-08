set(EXENAME http-client-json)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

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

set(LIBRARIES_SUB
    # Boost
    Boost::system
    Boost::filesystem

    ModuleManager
    SettingsManager
)

if(WIN32)
    set(LIBRARIES_SUB ${LIBRARIES_SUB}
        # Windows Libraries

    )
elseif(UNIX)
    set(LIBRARIES_SUB ${LIBRARIES_SUB}
        # Unix Libraries
    )
endif()

target_link_libraries(${EXENAME} ${LIBRARIES_SUB})
