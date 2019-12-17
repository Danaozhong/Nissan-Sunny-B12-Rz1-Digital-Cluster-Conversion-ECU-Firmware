################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/stm32f3xx_hal_msp.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/stm32f3xx_it.c \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/system_stm32f3xx.c 

CPP_SRCS += \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/main.cpp 

OBJS += \
./Src/main.o \
./Src/stm32f3xx_hal_msp.o \
./Src/stm32f3xx_it.o \
./Src/system_stm32f3xx.o 

C_DEPS += \
./Src/stm32f3xx_hal_msp.d \
./Src/stm32f3xx_it.d \
./Src/system_stm32f3xx.d 

CPP_DEPS += \
./Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Src/main.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/main.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Src/main.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32f3xx_hal_msp.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/stm32f3xx_hal_msp.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/stm32f3xx_hal_msp.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32f3xx_it.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/stm32f3xx_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/stm32f3xx_it.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/system_stm32f3xx.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Src/system_stm32f3xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Src/system_stm32f3xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

