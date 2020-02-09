CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

PROJECT(SunnyRz1FuelGaugeConverter  VERSION 1.0 DESCRIPTION "Test")


# some options
option(USE_CAN "Enable / disable the CAN subsystem" OFF)
option(USE_TRACE "Enable / disable the tracing" ON)
option(USE_NVDH "Enable / disable non-volatile data handler" ON)


ENABLE_LANGUAGE(ASM)

FIND_PACKAGE(CMSIS REQUIRED)
FIND_PACKAGE(STM32HAL COMPONENTS gpio tim adc dac can dma uart flash REQUIRED)


if(${STM32_FAMILY} STREQUAL "F3")
    add_definitions(
        -DSTM32_FAMILY_F3
        -DSTM32F303xC
    )
elseif(${STM32_FAMILY} STREQUAL "F4")
    add_definitions(
        -DSTM32_FAMILY_F4
        -DSTM32F429xx
    )
endif()

add_definitions(
    -D_BSD_SOURCE
    -D_GCC_MULTITHREAD_FREERTOS_ENABLE
    -DconfigRECORD_STACK_HIGH_ADDRESS=1
    -DDEBUG
    -DUSE_HAL_DRIVER
)

SET(PROJECT_SOURCES
    main.cpp
    stm32f3xx_it.c
    #system_stm32f3xx.c
    app/console_commands/src/console_commands.cpp
    app/fuel_gauge_input/src/fuel_gauge_input.cpp
    app/fuel_gauge_output/src/fuel_gauge_output.cpp
    app/lookup_table/src/lookup_table.cpp
    app/main_application/src/main_application.cpp
    app/speed_sensor_converter/src/speed_sensor_converter.cpp
    app/replay_curve/src/replay_curve.cpp
    
    drivers/adc/src/stm32_adc.cpp
    drivers/dac/src/generic_dac.cpp
    drivers/dac/src/stm32_dac.cpp
    drivers/eeprom/src/eeprom.c
    drivers/pwm/src/stm32_pwm.cpp
    drivers/pwm_ic/src/stm32_pwm_ic.cpp
    drivers/uart/src/generic_uart.cpp
    drivers/uart/src/stm32_uart.cpp
    drivers/uc_ports/src/uc_ports.cpp
    
    midware/cppstdlib/condition_variable.cpp
    midware/cppstdlib/freertos_time.cpp
    midware/cppstdlib/future.cc
    midware/cppstdlib/gthr_key.cpp
    midware/cppstdlib/mutex.cc
    midware/cppstdlib/thread.cpp
    
    midware/events/event_handler.cpp
    midware/excp_handler/src/excp_handler.cpp
    midware/excp_handler/src/excp_handler_console_commands.cpp
    
    midware/osservices/os_console/src/os_console.cpp
    midware/std_ex/ex_threads/ex_thread.cpp

    midware/util_algorithms/src/util_algorithms.cpp
    
    midware/FreeRTOS/adapt/CMSIS_RTOS/cmsis_os.c
    midware/FreeRTOS/src/croutine.c
    midware/FreeRTOS/src/event_groups.c
    midware/FreeRTOS/src/list.c
    midware/FreeRTOS/src/queue.c
    midware/FreeRTOS/src/stream_buffer.c
    midware/FreeRTOS/src/tasks.c
    midware/FreeRTOS/src/timers.c

    midware/FreeRTOS/src/portable/GCC/ARM_CM4F/port.c
    midware/FreeRTOS/src/portable/MemMang/heap_4.c
    
    misc/ascii_arts/src/ascii_diagram.cpp
)

if (USE_TRACE)
    list(APPEND PROJECT_SOURCES
        midware/trace/src/trace.cpp
    )
endif()

if (USE_NVDH)
    list(APPEND PROJECT_SOURCES
        midware/nonvolatile_data/src/nonvolatile_data_handler.cpp
    )
endif()

add_subdirectory(can_stack/autosar)
add_subdirectory(midware/trace)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES} ${CMSIS_SOURCES} ${STM32HAL_SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/console_commands/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/fuel_gauge_input/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/fuel_gauge_output/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/lookup_table/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/main_application/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/speed_sensor_converter/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/replay_curve/inc
        
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/adc/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/dac/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/eeprom/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pwm/inc
    drivers/pwm_ic/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/uart/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/uc_ports/inc
    
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/cppstdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/events
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/excp_handler/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/osservices/os_console/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/std_ex/ex_threads
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/util_algorithms/inc
    
    
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/adapt
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/adapt/CMSIS_RTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/src/freertos
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/src
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/src/portable/GCC/ARM_CM4F
    
    misc/ascii_arts/inc
)

# Link the LibFort library (used for ASCII table drawing)
add_subdirectory(misc/libs/libfort)
target_link_libraries(${CMAKE_PROJECT_NAME} fort)

if (USE_CAN)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_CAN
        -DHAL_CAN_MODULE_ENABLED
    )
    
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC LIB_CAN)
endif()

if (USE_TRACE)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_TRACE
    )
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        midware/trace/inc
    )
endif()

if (USE_NVDH)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_NVDH
    )
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        midware/nonvolatile_data/inc
    )
endif()
    
STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})
