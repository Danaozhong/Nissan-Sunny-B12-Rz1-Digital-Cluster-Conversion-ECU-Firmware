
set(MODULE_SOURCES
    src/stm32_adc.cpp
)
add_library(adc STATIC ${MODULE_SOURCES})

target_include_directories(adc PUBLIC
    inc
)