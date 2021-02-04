
SET(MODULE_SOURCES
    src/stdlib.cpp
)

add_library(stdlib STATIC ${MODULE_SOURCES})

target_link_libraries(stdlib PUBLIC
    stm32_peripherals
    freertos
    trace
)
