################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Startup/startup_stm32f303k8tx.s 

OBJS += \
./Startup/startup_stm32f303k8tx.o 


# Each subdirectory must supply rules for building sources it contributes
Startup/startup_stm32f303k8tx.o: H:/Projekte/STM32Workspace/sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303/Startup/startup_stm32f303k8tx.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DUSE_STM32FXX_NUCLEO_32 -DUSE_STM32F3XX_NUCLEO_32 -D_GCC_MULTITHREAD_FREERTOS_ENABLE -c -I../ -I../Src/midware/FreeRTOS/src -I../Src/drivers/dac/inc -I../Src/drivers/adc/inc -I../Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F -I../Src/midware/FreeRTOS/adapt -I../Src/midware/cppstdlib -I../Src/midware/events -I../Src/midware/trace -I../Src/midware/FreeRTOS/src/freertos -I../Src/app/lookup_table/inc -I../Src/app/fuel_gauge_input/inc -I../Src/app/fuel_gauge_output/inc -I../Src/drivers/uart/inc -I../Src/midware/osservices/os_console/inc -I../Drivers/BSP/STM32F3xx_Nucleo_32 -I"../Src/midware/std_ex/ex_threads" -x assembler-with-cpp --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

