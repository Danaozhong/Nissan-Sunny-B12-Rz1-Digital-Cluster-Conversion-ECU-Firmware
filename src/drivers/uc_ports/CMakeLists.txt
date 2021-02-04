
SET(MODULE_SOURCES
    src/uc_ports.cpp
)

add_library(uc_ports STATIC ${MODULE_SOURCES})

target_include_directories(uc_ports PUBLIC
    inc
)