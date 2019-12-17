################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/midware/trace/subdir.mk
-include Src/midware/std_ex/ex_threads/subdir.mk
-include Src/midware/osservices/os_console/src/subdir.mk
-include Src/midware/events/subdir.mk
-include Src/midware/FreeRTOS/src/portable/MemMang/subdir.mk
-include Src/midware/FreeRTOS/src/portable/GCC/ARM_CM4F/subdir.mk
-include Src/midware/FreeRTOS/src/subdir.mk
-include Src/midware/FreeRTOS/adapt/CMSIS_RTOS/subdir.mk
-include Src/drivers/uart/src/subdir.mk
-include Src/drivers/dac/src/subdir.mk
-include Src/drivers/adc/src/subdir.mk
-include Src/app/lookup_table/src/subdir.mk
-include Src/app/fuel_gauge_output/src/subdir.mk
-include Src/app/fuel_gauge_input/src/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F3xx_HAL_Driver/Src/subdir.mk
-include Drivers/BSP/STM32F3xx_Nucleo_32/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.list \


# All Target
all: NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.elf secondary-outputs

# Tool invocations
NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.elf: $(OBJS) $(USER_OBJS) H:\Projekte\STM32Workspace\sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303\prj\STM32F303K8TX_FLASH.ld
	arm-none-eabi-g++ -o "NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"H:\Projekte\STM32Workspace\sw.tool.sunny_rz1_fuel_gauge_converter_stm32f303\prj\STM32F303K8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "NissanSunnyRz1DigitalFuelGaugeSignalConverterSTM32F303Nucleo32.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
