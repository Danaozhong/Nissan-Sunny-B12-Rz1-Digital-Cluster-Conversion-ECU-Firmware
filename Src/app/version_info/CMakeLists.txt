


set(MODULE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/src/version_info.cpp
)

configure_file(src/version_info.cpp.in src/version_info.cpp @ONLY)

add_library(version_info STATIC ${MODULE_SOURCES})

set_target_properties(version_info PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(version_info PRIVATE
    BUILD_TYPE_Release=0
    BUILD_TYPE_Debug=1
    VERSION_INFO_BUILD_TYPE=BUILD_TYPE_${CMAKE_BUILD_TYPE}
)

target_include_directories(version_info PUBLIC
    inc
)