################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f030c8tx.s 

OBJS += \
./Core/Startup/startup_stm32f030c8tx.o 

S_DEPS += \
./Core/Startup/startup_stm32f030c8tx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0 -g3 -DDEBUG -c -I"C:/Users/ethan/STM32CubeIDE/workspace_1.17.0/CMS_Lantern_Firmware/lanternFirmwareCMS/MyWork" -I"C:/Users/ethan/STM32CubeIDE/workspace_1.17.0/CMS_Lantern_Firmware/lanternFirmwareCMS/Drivers/STM32F0xx_HAL_Driver" -I"C:/Users/ethan/STM32CubeIDE/workspace_1.17.0/CMS_Lantern_Firmware/lanternFirmwareCMS/Core/Src" -I"C:/Users/ethan/STM32CubeIDE/workspace_1.17.0/CMS_Lantern_Firmware/lanternFirmwareCMS/Core/Inc" -I"C:/Users/ethan/STM32CubeIDE/workspace_1.17.0/CMS_Lantern_Firmware/lanternFirmwareCMS/Core" -I"C:/Users/ethan/STM32CubeIDE/workspace_1.17.0/CMS_Lantern_Firmware/lanternFirmwareCMS/Drivers" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f030c8tx.d ./Core/Startup/startup_stm32f030c8tx.o

.PHONY: clean-Core-2f-Startup

