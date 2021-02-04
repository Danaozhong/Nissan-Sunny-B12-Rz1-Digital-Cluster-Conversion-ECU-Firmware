
SET(MODULE_SOURCES
    src/generic_uart.cpp
    src/stm32_uart.cpp
)


add_library(uart STATIC ${MODULE_SOURCES})

target_link_libraries(uart PUBLIC
    trace
)

target_include_directories(uart PUBLIC
    inc
)