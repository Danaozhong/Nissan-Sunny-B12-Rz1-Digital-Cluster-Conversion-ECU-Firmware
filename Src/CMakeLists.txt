PROJECT(SunnyRz1FuelGaugeConverter)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_LANGUAGE(ASM)

FIND_PACKAGE(CMSIS REQUIRED)
FIND_PACKAGE(STM32HAL COMPONENTS gpio tim adc dac can dma uart flash REQUIRED)

add_definitions(
    -D_BSD_SOURCE
    -D_GCC_MULTITHREAD_FREERTOS_ENABLE
    -DconfigRECORD_STACK_HIGH_ADDRESS=1
    -DDEBUG
    -DSTM32F303xC
    -DUSE_HAL_DRIVER
)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/console_commands/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/fuel_gauge_input/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/fuel_gauge_output/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/lookup_table/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/main_application/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/speed_sensor_converter/inc
    
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/Can
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/CanIf
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/config
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/Det
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/EcuM
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/PduR
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/SchM
    ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/shared
    
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/adc/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/dac/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/eeprom/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pwm/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/uart/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/uc_ports/inc
    
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/cppstdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/events
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/excp_handler/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/osservices/os_console/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/std_ex/ex_threads
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/trace/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/util_algorithms/inc
    
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/adapt
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/adapt/CMSIS_RTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/src/freertos
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/src
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/FreeRTOS/src/portable/GCC/ARM_CM4F
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
    can_stack/autosar/Can/Can.c
    can_stack/autosar/CanIf/CanIf.c
    can_stack/autosar/CanIf/CanIf_Cfg.c
    can_stack/autosar/CanIf/CanIf_Lcfg.c
    can_stack/autosar/config/Can_Lcfg.c
    can_stack/autosar/Det/Det.c
    drivers/adc/src/stm32_adc.cpp
    drivers/dac/src/generic_dac.cpp
    drivers/dac/src/stm32_dac.cpp
    drivers/eeprom/src/eeprom.c
    drivers/pwm/src/stm32_pwm.cpp
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
    midware/trace/src/trace.cpp
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
)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES} ${CMSIS_SOURCES} ${STM32HAL_SOURCES})

STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})
