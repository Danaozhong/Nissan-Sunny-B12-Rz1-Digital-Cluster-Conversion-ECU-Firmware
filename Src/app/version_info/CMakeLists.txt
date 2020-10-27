


set(MODULE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/src/version_info.cpp
)

configure_file(src/version_info.cpp.in src/version_info.cpp @ONLY)

add_library(version_info STATIC ${MODULE_SOURCES})

set_target_properties(version_info PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(version_info PUBLIC
    inc
)