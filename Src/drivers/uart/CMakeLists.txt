
SET(MODULE_SOURCES
    src/generic_uart.cpp
    src/stm32_uart.cpp
)


add_library(uart STATIC ${MODULE_SOURCES})

target_link_libraries(uart stdcpp)
target_link_libraries(uart stdcppex)


#target_include_directories(can_stack PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMSIS_INCLUDE_DIRS}
#    ${STM32HAL_INCLUDE_DIR}
#    ../../Inc # needed for HW depending stuff, TODO find a better solution for this include
#)
#add_definitions(
#    -D_BSD_SOURCE
#    -D_GCC_MULTITHREAD_FREERTOS_ENABLE
#    -DconfigRECORD_STACK_HIGH_ADDRESS=1
#    -DDEBUG
#    -DUSE_HAL_DRIVER
#        -DSTM32_FAMILY_F3
#        -DSTM32F303xC
#)

#target_include_directories(uart PUBLIC
#    ${CMSIS_INCLUDE_DIRS}
#    ${STM32HAL_INCLUDE_DIR}
#    ../cppstdlib
#    ../FreeRTOS/adapt
#    ../FreeRTOS/adapt/CMSIS_RTOS
#    ../FreeRTOS/src/freertos
#    ../FreeRTOS/src
#    ../FreeRTOS/src/portable/GCC/ARM_CM4F
#)

target_include_directories(uart PUBLIC
    inc
)