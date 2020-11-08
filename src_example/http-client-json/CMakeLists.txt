set(EXENAME http-client-json)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(SOURCES
    main.cpp
)

set(HEADERS
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

add_executable(${EXENAME} ${SOURCES})

set(LIBRARIES
    # Boost
    Boost::system
    Boost::filesystem

    ModuleManager
    SettingsManager
)

if(WIN32)
    set(LIBRARIES ${LIBRARIES}
        # Windows Libraries

    )
elseif(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
    set(LIBRARIES ${LIBRARIES}
        # Unix Libraries
        dl
    )
endif()

target_link_libraries(${EXENAME} ${LIBRARIES})
