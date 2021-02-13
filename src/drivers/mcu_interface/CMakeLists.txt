
SET(MODULE_SOURCES
    src/mcu_interface.cpp
)

add_library(mcu_interface STATIC ${MODULE_SOURCES})

target_link_libraries(mcu_interface PUBLIC
    stm32_peripherals
    CMSIS::STM32::F303CC
    STM32::Nano
)

target_include_directories(mcu_interface PUBLIC
    inc
)
