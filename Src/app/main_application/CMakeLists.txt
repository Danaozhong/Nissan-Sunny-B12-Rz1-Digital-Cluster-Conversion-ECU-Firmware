
set(MODULE_SOURCES
    src/main_application.cpp
)
add_library(main_application STATIC ${MODULE_SOURCES})

target_link_libraries(main_application PUBLIC
    fuel_gauge_input
    fuel_gauge_output
    osservices
    lookup_table_editor
    speed_sensor_converter
    dataset
    eol
    eol_command
    console_commands
    pwm_tester
    os_threads
)

target_include_directories(main_application PUBLIC
    inc
)