
set(MODULE_SOURCES
    src/stm32_pwm_ic.cpp
)
add_library(pwm_ic STATIC ${MODULE_SOURCES})

target_link_libraries(pwm_ic osservices)

target_include_directories(pwm_ic PUBLIC
    inc
)