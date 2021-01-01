
set(MODULE_SOURCES
    src/stm32_pwm.cpp
)
add_library(pwm STATIC ${MODULE_SOURCES})

target_include_directories(pwm PUBLIC
    inc
)