CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/stm32_gcc.cmake)

PROJECT(SunnyRz1FuelGaugeConverter VERSION 1.0 DESCRIPTION "The firmware of an ECU to convert a Nissan Rz1 to digital cluster" LANGUAGES ASM C CXX)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
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

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CMSIS COMPONENTS STM32F303CCT6 REQUIRED)
find_package(HAL COMPONENTS STM32F303CCT6 REQUIRED)
find_library(GCCLIBSTDCXXNANO libstdc++_nano.a REQUIRED)
  

add_definitions(
    -DSTM32_FAMILY_F3
    -DSTM32F303xC
)
add_definitions(
    -D_BSD_SOURCE
    -D_GCC_MULTITHREAD_FREERTOS_ENABLE
    -DconfigRECORD_STACK_HIGH_ADDRESS=1
    -DDEBUG
    -DUSE_HAL_DRIVER
)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(
        -DDEBUG
    )
endif()

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
   
    drivers/adc/src/stm32_adc.cpp
    drivers/dac/src/generic_dac.cpp
    drivers/dac/src/stm32_dac.cpp
    drivers/pwm/src/stm32_pwm.cpp
    drivers/pwm_ic/src/stm32_pwm_ic.cpp
    drivers/uart/src/generic_uart.cpp
    drivers/uart/src/stm32_uart.cpp
    drivers/uc_ports/src/uc_ports.cpp
    
    midware/events/event_handler.cpp
    midware/std_ex/ex_threads/ex_thread.cpp
)



add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})

# Make sure the file extension of the created binary is *.elf
set_target_properties(
    ${CMAKE_PROJECT_NAME} 
    PROPERTIES 
        SUFFIX ".elf"
)


add_compile_options(
    -fno-exceptions
    -pedantic
)

# global includes and libraries
include_directories(
    midware/cppstdlib/cpp11_gcc  # This include directory must come first, so that the freertos cpp11 libs work.
    ../Inc/ # This includes the STM32 configuration headers.
)

add_subdirectory(midware/cppstdlib)
add_subdirectory(midware/FreeRTOS)
add_subdirectory(midware/std_ex)

link_libraries(
    CMSIS::STM32::F303CC
    STM32::Nano
)

link_libraries(    
    freertos
    cppstdlib
    stdcppex)

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
    app/dataset/inc
        
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/adc/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/dac/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pwm/inc
    drivers/pwm_ic/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/uart/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/uc_ports/inc
    
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/cppstdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/events
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/osservices/os_console/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/midware/std_ex/ex_threads
)





add_subdirectory(midware/osservices)
list(APPEND EXTRA_LIBS osservices)

add_subdirectory(midware/util_algorithms)
list(APPEND EXTRA_LIBS util_algorithms)

add_subdirectory(drivers/uart)
list(APPEND EXTRA_LIBS uart)

add_subdirectory(drivers/eeprom)
list(APPEND EXTRA_LIBS eeprom)

add_subdirectory(misc/ascii_graph)
list(APPEND EXTRA_LIBS ascii_graph)

add_subdirectory(misc/libs/libtable)
list(APPEND EXTRA_LIBS libtable)


add_subdirectory(app/version_info)
list(APPEND EXTRA_LIBS version_info)

add_subdirectory(app/eol)
list(APPEND EXTRA_LIBS eol)

add_subdirectory(app/eol_command)
list(APPEND EXTRA_LIBS eol_command)

if (USE_NVDH)
    add_subdirectory(midware/nonvolatile_data)
    list(APPEND EXTRA_LIBS nonvolatile_data_handler)
endif()

if (USE_TRACE)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_TRACE
    )
    add_subdirectory(midware/trace)
    list(APPEND EXTRA_LIBS trace)
endif()

add_subdirectory(midware/excp_handler)
list(APPEND EXTRA_LIBS excp_handler)

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
    
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC 
    HAL::STM32::F3::GPIO
    HAL::STM32::F3::TIM
    HAL::STM32::F3::TIMEx
    HAL::STM32::F3::ADC
    HAL::STM32::F3::ADCEx
    HAL::STM32::F3::DAC
    HAL::STM32::F3::CAN
    HAL::STM32::F3::DMA
    HAL::STM32::F3::UART
    HAL::STM32::F3::UARTEx
    HAL::STM32::F3::FLASH
    HAL::STM32::F3::FLASHEx
    HAL::STM32::F3::RCC
    HAL::STM32::F3::RCCEx
    HAL::STM32::F3::CORTEX
    #libstdc++_nano.a
    
    #D:/Programs/STM32CubeIDE/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.win32_1.0.0.201904181610/tools/arm-none-eabi/lib/libstdc++_nano.a
    ${EXTRA_LIBS}
)

target_link_options(${CMAKE_PROJECT_NAME} PUBLIC 
    -Wl,-Map=${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/${CMAKE_PROJECT_NAME}.map
    -Wl,-t
    -flto
    #-nodefaultlibs
)
#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")


get_target_property(OUT ${CMAKE_PROJECT_NAME} LINK_LIBRARIES)
message(STATUS ${OUT})

#add_custom_target(graphviz ALL
#                  COMMAND "${CMAKE_COMMAND}" "--graphviz=foo.dot" .
#                  COMMAND D:/Programme/GraphWiz/Graphviz/bin/dot -Tpng foo.dot -o foo.png
#                  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
                  
#STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})
#STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})

# Print output section size
add_custom_command(
  OUTPUT print_size
  POST_BUILD COMMAND ${TOOLCHAIN_BIN_PATH}/arm-none-eabi-size ${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/${CMAKE_PROJECT_NAME}.elf 
  DEPENDS ${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/${CMAKE_PROJECT_NAME}.elf
  COMMENT "Binary size:\n\n"
)

add_custom_target(size ALL DEPENDS print_size) 

