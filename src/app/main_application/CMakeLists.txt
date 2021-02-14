

option(APP_MEASURE_STARTUP_TIME "Measure the startup time and print it out on the UART" ON)

# Specify how many flash blocks to use to write nonvolatile data. Each block has a size of 1024
set(APP_NUM_OF_NVM_BLOCKS_TO_USE 1)


set(MODULE_SOURCES
    src/main_application.cpp
    src/cyclic_thread_100ms.cpp
    src/cyclic_thread_low_prio_100ms.cpp
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
    watchdog
    mcu_interface
)

target_include_directories(main_application PUBLIC
    inc
)

if (APP_MEASURE_STARTUP_TIME)
    target_compile_definitions(main_application PRIVATE
        -DMAIN_APPLICATION_MEASURE_STARTUP_TIME
    )
endif()

target_compile_definitions(main_application PRIVATE
    APP_NUM_OF_NVM_BLOCKS_TO_USE=${APP_NUM_OF_NVM_BLOCKS_TO_USE}
)

