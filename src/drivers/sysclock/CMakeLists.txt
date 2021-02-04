
SET(MODULE_SOURCES
    src/sysclock.cpp
)

add_library(sysclock STATIC ${MODULE_SOURCES})

target_link_libraries(sysclock PUBLIC
    stm32_peripherals
    CMSIS::STM32::F303CC
    STM32::Nano
)

target_include_directories(sysclock PUBLIC
    inc
)
