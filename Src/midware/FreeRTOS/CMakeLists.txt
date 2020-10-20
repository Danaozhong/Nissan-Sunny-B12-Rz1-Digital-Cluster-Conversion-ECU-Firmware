
SET(MODULE_SOURCES
    adapt/CMSIS_RTOS/cmsis_os.c
    src/croutine.c
    src/event_groups.c
    src/list.c
    src/queue.c
    src/stream_buffer.c
    src/tasks.c
    src/timers.c
    
    src/portable/GCC/ARM_CM4F/port.c
    src/portable/MemMang/heap_4.c
)


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

add_library(freertos STATIC ${MODULE_SOURCES})

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

target_include_directories(freertos PUBLIC
    adapt
    adapt/CMSIS_RTOS
    src/freertos
    src
    src/portable/GCC/ARM_CM4F
)

target_include_directories(freertos PUBLIC
    ../../../Inc
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}

)
