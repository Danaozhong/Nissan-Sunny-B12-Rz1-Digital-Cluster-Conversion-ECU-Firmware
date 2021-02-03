
SET(MODULE_SOURCES
    src/trace.cpp
)


add_library(trace STATIC ${MODULE_SOURCES})

target_link_libraries(trace PUBLIC
    uart
    osservices # Requires information about the status of the OS console (outputting is enabled / disabled )
)  

target_compile_definitions(trace PUBLIC
    USE_TRACE
)

target_compile_definitions(trace PUBLIC
    _BSD_SOURCE
    _GCC_MULTITHREAD_FREERTOS_ENABLE
    configRECORD_STACK_HIGH_ADDRESS=1
    USE_HAL_DRIVER
    STM32_FAMILY_F3
    STM32F303xC
)

target_include_directories(trace PUBLIC
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
    ../cppstdlib
    ../FreeRTOS/adapt
    ../FreeRTOS/adapt/CMSIS_RTOS
    ../FreeRTOS/src/freertos
    ../FreeRTOS/src
    ../FreeRTOS/src/portable/GCC/ARM_CM4F
)

target_include_directories(trace PUBLIC
    inc
)