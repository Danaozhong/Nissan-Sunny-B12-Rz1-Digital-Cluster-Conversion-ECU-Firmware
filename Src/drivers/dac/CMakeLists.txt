
set(MODULE_SOURCES
    src/generic_dac.cpp
    src/stm32_dac.cpp
)
add_library(dac STATIC ${MODULE_SOURCES})

target_include_directories(dac PUBLIC
    inc
)