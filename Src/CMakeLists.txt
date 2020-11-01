CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

PROJECT(SunnyRz1FuelGaugeConverter VERSION 1.0 DESCRIPTION "The firmware of an ECU to convert a Nissan Rz1 to digital cluster" LANGUAGES C CXX)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# some options
option(USE_CAN "Enable / disable the CAN subsystem" OFF)
option(USE_TRACE "Enable / disable the tracing" ON)
option(USE_NVDH "Enable / disable non-volatile data handler" ON)

# Get the current Git commit
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../tools/cmake/")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_local_changes(GIT_LOCAL_CHANGES)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE})

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
    app/lookup_table_editor/src/lookup_table_editor.cpp
    app/main_application/src/main_application.cpp
    app/speed_sensor_converter/src/speed_sensor_converter.cpp
    app/replay_curve/src/replay_curve.cpp
    app/dataset/src/dataset.cpp
    
    app/can_diagnostics/src/can_diagnostics.cpp
    
    drivers/adc/src/stm32_adc.cpp
    drivers/dac/src/generic_dac.cpp
    drivers/dac/src/stm32_dac.cpp
    drivers/eeprom/src/eeprom.c
    drivers/pwm/src/stm32_pwm.cpp
    drivers/pwm_ic/src/stm32_pwm_ic.cpp
    drivers/uart/src/generic_uart.cpp
    drivers/uart/src/stm32_uart.cpp
    drivers/uc_ports/src/uc_ports.cpp
    

    
    midware/events/event_handler.cpp
    midware/excp_handler/src/excp_handler.cpp
    midware/excp_handler/src/excp_handler_console_commands.cpp
    

    midware/std_ex/ex_threads/ex_thread.cpp

    midware/util_algorithms/src/util_algorithms.cpp
    

    
    misc/ascii_arts/src/ascii_diagram.cpp
)



if (USE_NVDH)
    list(APPEND PROJECT_SOURCES
        midware/nonvolatile_data/src/nonvolatile_data_handler.cpp
    )
endif()

add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES} ${CMSIS_SOURCES} ${STM32HAL_SOURCES})

# Make sure the file extension of the created binary is *.elf
set_target_properties(
    ${CMAKE_PROJECT_NAME} 
    PROPERTIES 
        SUFFIX ".elf"
)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/app/lookup_table_editor/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/main_application/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/speed_sensor_converter/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/replay_curve/inc
    
    app/can_diagnostics/inc
    
    app/dataset/inc
        
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

    misc/ascii_arts/inc
)


add_subdirectory(midware/FreeRTOS)
list(APPEND EXTRA_LIBS freertos)

add_subdirectory(midware/cppstdlib)
list(APPEND EXTRA_LIBS stdcpp)

add_subdirectory(midware/std_ex)
list(APPEND EXTRA_LIBS stdcppex)

add_subdirectory(midware/osservices)
list(APPEND EXTRA_LIBS osservices)

add_subdirectory(misc/libs/libtable)
list(APPEND EXTRA_LIBS libtable)

add_subdirectory(drivers/uart)
list(APPEND EXTRA_LIBS uart)

add_subdirectory(app/version_info)
list(APPEND EXTRA_LIBS version_info)



if (USE_TRACE)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_TRACE
    )
    add_subdirectory(midware/trace)
    list(APPEND EXTRA_LIBS trace)
endif()

if (USE_CAN)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_CAN
        -DHAL_CAN_MODULE_ENABLED
    )
    add_subdirectory(can_stack)
    list(APPEND EXTRA_LIBS can_stack)
    
    
    add_subdirectory(app/ima_data_provider)
    list(APPEND EXTRA_LIBS ima_data_provider)
    
   #list(APPEND EXTRA_INCLUDES 
   #     ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/Can
   #     ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/CanIf
   #     ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/config
  #      ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/Det
  #      ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/shared
  #  )
    
    
endif()

if (USE_NVDH)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_NVDH
    )
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        midware/nonvolatile_data/inc
    )
endif()
    
    
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${EXTRA_LIBS})

STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})
