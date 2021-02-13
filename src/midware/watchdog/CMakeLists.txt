
SET(MODULE_SOURCES
    src/watchdog.cpp
)


add_library(watchdog STATIC ${MODULE_SOURCES})

target_link_libraries(watchdog 
    PUBLIC
    stm32_peripherals
    HAL::STM32::F3::IWDG
)

target_include_directories(watchdog PUBLIC
    inc
)

target_compile_definitions(watchdog PUBLIC
    -DHAL_IWDG_MODULE_ENABLED
)