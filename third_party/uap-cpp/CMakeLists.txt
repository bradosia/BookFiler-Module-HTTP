include_directories(
    uap-cpp/yaml-cpp/include
)

link_directories(
)

# Set up source files
set(SOURCES
    uap-cpp/UaParser.cpp
    uap-cpp/internal/AlternativeExpander.cpp
    uap-cpp/internal/Pattern.cpp
    uap-cpp/internal/ReplaceTemplate.cpp
    uap-cpp/internal/SnippetIndex.cpp
)

set(HEADERS
    uap-cpp/UaParser.cpp
    uap-cpp/internal/AlternativeExpander.h
    uap-cpp/internal/Pattern.h
    uap-cpp/internal/ReplaceTemplate.h
    uap-cpp/internal/SnippetIndex.h

    uap-cpp/internal/MakeUnique.h
    uap-cpp/internal/SnippetMapping.h
    uap-cpp/internal/StringUtils.h
    uap-cpp/internal/StringView.h
)

set(LIBRARIES_SUB ${LIBRARIES_SUB}
    yaml-cpp
    #pthread
)

set(STATIC_LINK_LIBRARIES ${LIBRARIES_SUB})

# THIRD PARTY
add_subdirectory(uap-cpp/yaml-cpp)

set(uap-cpp-lib-name uap-cpp)
message( STATUS "-- Building ${lib_name}" )
add_library(${uap-cpp-lib-name} STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${uap-cpp-lib-name} ${STATIC_LINK_LIBRARIES})

if(UNIX)
	set_target_properties(${uap-cpp-lib-name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
