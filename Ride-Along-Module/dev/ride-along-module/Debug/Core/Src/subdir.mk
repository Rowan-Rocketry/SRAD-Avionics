################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/fileio.c \
../Core/Src/logging.c \
../Core/Src/lsm6dsl.c \
../Core/Src/main.c \
../Core/Src/ms5607.c \
../Core/Src/stm32u5xx_hal_msp.c \
../Core/Src/stm32u5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32u5xx.c 

OBJS += \
./Core/Src/fileio.o \
./Core/Src/logging.o \
./Core/Src/lsm6dsl.o \
./Core/Src/main.o \
./Core/Src/ms5607.o \
./Core/Src/stm32u5xx_hal_msp.o \
./Core/Src/stm32u5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32u5xx.o 

C_DEPS += \
./Core/Src/fileio.d \
./Core/Src/logging.d \
./Core/Src/lsm6dsl.d \
./Core/Src/main.d \
./Core/Src/ms5607.d \
./Core/Src/stm32u5xx_hal_msp.d \
./Core/Src/stm32u5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32u5xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U545xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I"/home/sharpe/Documents/Rowan/Rowan-Projects/Rowan-Rocketry/SRAD-Avionics/Ride-Along-Module/dev/ride-along-module/Libraries/Third_Party/FatFs/src" -I"/home/sharpe/Documents/Rowan/Rowan-Projects/Rowan-Rocketry/SRAD-Avionics/Ride-Along-Module/dev/ride-along-module/FATFS/App" -I"/home/sharpe/Documents/Rowan/Rowan-Projects/Rowan-Rocketry/SRAD-Avionics/Ride-Along-Module/dev/ride-along-module/FATFS/Target" -O0 -ffunction-sections -fdata-sections -Wall -Wmissing-include-dirs -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/fileio.cyclo ./Core/Src/fileio.d ./Core/Src/fileio.o ./Core/Src/fileio.su ./Core/Src/logging.cyclo ./Core/Src/logging.d ./Core/Src/logging.o ./Core/Src/logging.su ./Core/Src/lsm6dsl.cyclo ./Core/Src/lsm6dsl.d ./Core/Src/lsm6dsl.o ./Core/Src/lsm6dsl.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/ms5607.cyclo ./Core/Src/ms5607.d ./Core/Src/ms5607.o ./Core/Src/ms5607.su ./Core/Src/stm32u5xx_hal_msp.cyclo ./Core/Src/stm32u5xx_hal_msp.d ./Core/Src/stm32u5xx_hal_msp.o ./Core/Src/stm32u5xx_hal_msp.su ./Core/Src/stm32u5xx_it.cyclo ./Core/Src/stm32u5xx_it.d ./Core/Src/stm32u5xx_it.o ./Core/Src/stm32u5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32u5xx.cyclo ./Core/Src/system_stm32u5xx.d ./Core/Src/system_stm32u5xx.o ./Core/Src/system_stm32u5xx.su

.PHONY: clean-Core-2f-Src

