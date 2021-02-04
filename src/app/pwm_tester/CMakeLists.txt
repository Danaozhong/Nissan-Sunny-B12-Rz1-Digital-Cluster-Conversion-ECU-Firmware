
set(MODULE_SOURCES
    src/pwm_tester.cpp
)
add_library(pwm_tester STATIC ${MODULE_SOURCES})

target_link_libraries(pwm_tester speed_sensor_converter)
target_link_libraries(pwm_tester excp_handler)
target_link_libraries(pwm_tester trace)


target_include_directories(pwm_tester PUBLIC
    inc
)