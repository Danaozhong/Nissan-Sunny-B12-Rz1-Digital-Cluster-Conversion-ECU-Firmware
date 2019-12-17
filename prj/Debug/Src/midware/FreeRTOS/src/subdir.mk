################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/croutine.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/event_groups.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/list.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/queue.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/stream_buffer.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/tasks.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/timers.c 

OBJS += \
./Src/midware/FreeRTOS/src/croutine.o \
./Src/midware/FreeRTOS/src/event_groups.o \
./Src/midware/FreeRTOS/src/list.o \
./Src/midware/FreeRTOS/src/queue.o \
./Src/midware/FreeRTOS/src/stream_buffer.o \
./Src/midware/FreeRTOS/src/tasks.o \
./Src/midware/FreeRTOS/src/timers.o 

C_DEPS += \
./Src/midware/FreeRTOS/src/croutine.d \
./Src/midware/FreeRTOS/src/event_groups.d \
./Src/midware/FreeRTOS/src/list.d \
./Src/midware/FreeRTOS/src/queue.d \
./Src/midware/FreeRTOS/src/stream_buffer.d \
./Src/midware/FreeRTOS/src/tasks.d \
./Src/midware/FreeRTOS/src/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Src/midware/FreeRTOS/src/croutine.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/croutine.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/midware/FreeRTOS/src/croutine.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/midware/FreeRTOS/src/event_groups.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/event_groups.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/midware/FreeRTOS/src/event_groups.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/midware/FreeRTOS/src/list.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/list.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/midware/FreeRTOS/src/list.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/midware/FreeRTOS/src/queue.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/queue.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/midware/FreeRTOS/src/queue.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/midware/FreeRTOS/src/stream_buffer.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/stream_buffer.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/midware/FreeRTOS/src/stream_buffer.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/midware/FreeRTOS/src/tasks.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/tasks.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/midware/FreeRTOS/src/tasks.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/midware/FreeRTOS/src/timers.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/midware/FreeRTOS/src/timers.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/midware/FreeRTOS/src/timers.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

