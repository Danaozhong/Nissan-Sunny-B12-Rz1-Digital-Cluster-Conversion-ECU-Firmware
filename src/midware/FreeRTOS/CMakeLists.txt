
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

add_library(freertos STATIC ${MODULE_SOURCES})

target_include_directories(freertos PUBLIC
    adapt
    adapt/CMSIS_RTOS
    src/include
    src
    src/portable/GCC/ARM_CM4F
)

target_link_libraries(freertos PUBLIC
    stm32_peripherals
    HAL::STM32::F3::CORTEX
)