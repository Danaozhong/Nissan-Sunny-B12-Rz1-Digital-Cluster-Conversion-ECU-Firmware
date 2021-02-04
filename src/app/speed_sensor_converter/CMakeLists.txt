
set(MODULE_SOURCES
    src/speed_sensor_converter.cpp
)
add_library(speed_sensor_converter STATIC ${MODULE_SOURCES})

target_link_libraries(speed_sensor_converter pwm)
target_link_libraries(speed_sensor_converter pwm_ic)
target_link_libraries(speed_sensor_converter excp_handler)
target_link_libraries(speed_sensor_converter replay_curve)

target_include_directories(speed_sensor_converter PUBLIC
    inc
)