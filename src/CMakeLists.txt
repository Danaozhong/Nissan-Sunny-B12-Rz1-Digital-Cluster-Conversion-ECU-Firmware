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

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
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
    -DUSE_HAL_DRIVER
    -DHAL_IWDG_MODULE_ENABLED
)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(
        -DDEBUG
    )
endif()

SET(PROJECT_SOURCES
    main.cpp
    stm32f3xx_it.c
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
)
add_subdirectory(drivers/stm32_peripherals)


add_subdirectory(midware/cppstdlib)
add_subdirectory(midware/FreeRTOS)
add_subdirectory(midware/std_ex)
add_subdirectory(midware/stdlib)

link_libraries(
    stm32_peripherals
    CMSIS::STM32::F303CC
    STM32::Nano
)

link_libraries(
    freertos
    cppstdlib
    stdcppex
    stdlib)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/app/lookup_table_editor/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/speed_sensor_converter/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/app/replay_curve/inc
    app/dataset/inc
)

add_subdirectory(drivers/mcu_interface)
list(APPEND EXTRA_LIBS mcu_interface)

if (USE_TRACE)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_TRACE
    )
    add_subdirectory(midware/trace)
    list(APPEND EXTRA_LIBS trace)
endif()

add_subdirectory(midware/osservices)
list(APPEND EXTRA_LIBS osservices)

add_subdirectory(midware/osservices/os_threads)
list(APPEND EXTRA_LIBS os_threads)

add_subdirectory(midware/event_handler)
list(APPEND EXTRA_LIBS event_handler)

add_subdirectory(midware/util_algorithms)
list(APPEND EXTRA_LIBS util_algorithms)

add_subdirectory(midware/watchdog)
list(APPEND EXTRA_LIBS watchdog)

add_subdirectory(drivers/uart)
list(APPEND EXTRA_LIBS uart)

add_subdirectory(drivers/adc)
list(APPEND EXTRA_LIBS adc)

add_subdirectory(drivers/dac)
list(APPEND EXTRA_LIBS dac)

add_subdirectory(drivers/pwm)
list(APPEND EXTRA_LIBS pwm)

add_subdirectory(drivers/pwm_ic)
list(APPEND EXTRA_LIBS pwm_ic)

add_subdirectory(drivers/eeprom)
list(APPEND EXTRA_LIBS eeprom)

add_subdirectory(drivers/uc_ports)
list(APPEND EXTRA_LIBS uc_ports)

add_subdirectory(misc/ascii_graph)
list(APPEND EXTRA_LIBS ascii_graph)

add_subdirectory(misc/libs/libtable)
list(APPEND EXTRA_LIBS libtable)


if (USE_NVDH)
    add_subdirectory(midware/nonvolatile_data)
    list(APPEND EXTRA_LIBS nonvolatile_data_handler)
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
endif()

if (USE_NVDH)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        -DUSE_NVDH
    )
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        midware/nonvolatile_data/inc
    )
endif()

add_subdirectory(app/version_info)
list(APPEND EXTRA_LIBS version_info)

add_subdirectory(app/lookup_table)
list(APPEND EXTRA_LIBS lookup_table)

add_subdirectory(app/fuel_gauge_input)
list(APPEND EXTRA_LIBS fuel_gauge_input)

add_subdirectory(app/fuel_gauge_output)
list(APPEND EXTRA_LIBS fuel_gauge_output)

add_subdirectory(app/replay_curve)
list(APPEND EXTRA_LIBS replay_curve)

add_subdirectory(app/speed_sensor_converter)
list(APPEND EXTRA_LIBS speed_sensor_converter)

add_subdirectory(app/dataset)
list(APPEND EXTRA_LIBS dataset)

add_subdirectory(app/eol)
list(APPEND EXTRA_LIBS eol)

add_subdirectory(app/eol_command)
list(APPEND EXTRA_LIBS eol_command)

add_subdirectory(app/lookup_table_editor)
list(APPEND EXTRA_LIBS lookup_table_editor)

add_subdirectory(app/pwm_tester)
list(APPEND EXTRA_LIBS pwm_tester)

add_subdirectory(app/main_application)
list(APPEND EXTRA_LIBS main_application)

add_subdirectory(app/console_commands)
list(APPEND EXTRA_LIBS console_commands)
    
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC 
    HAL::STM32::F3::GPIO
    HAL::STM32::F3::TIM
    HAL::STM32::F3::TIMEx
    HAL::STM32::F3::ADC
    HAL::STM32::F3::ADCEx
    HAL::STM32::F3::DACEx
    HAL::STM32::F3::CAN
    HAL::STM32::F3::DMA
    HAL::STM32::F3::UART
    HAL::STM32::F3::UARTEx
    HAL::STM32::F3::FLASH
    HAL::STM32::F3::FLASHEx
    HAL::STM32::F3::RCC
    HAL::STM32::F3::RCCEx
    HAL::STM32::F3::CORTEX
    ${EXTRA_LIBS}
)

target_link_options(${CMAKE_PROJECT_NAME} PUBLIC 
    -Wl,-Map=${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/${CMAKE_PROJECT_NAME}.map
    -Wl,-t
    -flto
)


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

