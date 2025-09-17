################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/Third_Party/FatFs/src/option/syscall.c 

OBJS += \
./Libraries/Third_Party/FatFs/src/option/syscall.o 

C_DEPS += \
./Libraries/Third_Party/FatFs/src/option/syscall.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/Third_Party/FatFs/src/option/%.o Libraries/Third_Party/FatFs/src/option/%.su Libraries/Third_Party/FatFs/src/option/%.cyclo: ../Libraries/Third_Party/FatFs/src/option/%.c Libraries/Third_Party/FatFs/src/option/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U545xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -Wmissing-include-dirs -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libraries-2f-Third_Party-2f-FatFs-2f-src-2f-option

clean-Libraries-2f-Third_Party-2f-FatFs-2f-src-2f-option:
	-$(RM) ./Libraries/Third_Party/FatFs/src/option/syscall.cyclo ./Libraries/Third_Party/FatFs/src/option/syscall.d ./Libraries/Third_Party/FatFs/src/option/syscall.o ./Libraries/Third_Party/FatFs/src/option/syscall.su

.PHONY: clean-Libraries-2f-Third_Party-2f-FatFs-2f-src-2f-option

