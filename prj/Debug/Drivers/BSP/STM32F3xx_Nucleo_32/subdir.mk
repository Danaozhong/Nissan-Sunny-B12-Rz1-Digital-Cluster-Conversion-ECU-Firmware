################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Drivers/BSP/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.c 

OBJS += \
./Drivers/BSP/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.o 

C_DEPS += \
./Drivers/BSP/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Drivers/BSP/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F303x8 -DDEBUG '-DconfigRECORD_STACK_HIGH_ADDRESS=1' -DUSE_STM32F3XX_NUCLEO_32 -c -I../../Inc -I../../Drivers/CMSIS/Include -I../../Drivers/STM32F3xx_HAL_Driver/Inc -I../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../../Src/midware/FreeRTOS/src -I../../Src/drivers/dac/inc -I../../Src/drivers/adc/inc -I../../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../../Src/midware/FreeRTOS/adapt -I../../Src/midware/cppstdlib -I../../Src/midware/events -I../../Src/midware/trace -I../../Src/midware/FreeRTOS/src/freertos -I../../Src/app/lookup_table/inc -I../../Src/app/fuel_gauge_input/inc -I../../Src/app/fuel_gauge_output/inc -I../../Src/drivers/uart/inc -I../../Src/midware/osservices/os_console/inc -I../../Drivers/BSP/STM32F3xx_Nucleo_32 -I../../Src/midware/std_ex/ex_threads -O0 -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
