################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/Third_Party/FatFs/src/diskio.c \
../Libraries/Third_Party/FatFs/src/ff.c \
../Libraries/Third_Party/FatFs/src/ff_gen_drv.c 

OBJS += \
./Libraries/Third_Party/FatFs/src/diskio.o \
./Libraries/Third_Party/FatFs/src/ff.o \
./Libraries/Third_Party/FatFs/src/ff_gen_drv.o 

C_DEPS += \
./Libraries/Third_Party/FatFs/src/diskio.d \
./Libraries/Third_Party/FatFs/src/ff.d \
./Libraries/Third_Party/FatFs/src/ff_gen_drv.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/Third_Party/FatFs/src/%.o Libraries/Third_Party/FatFs/src/%.su Libraries/Third_Party/FatFs/src/%.cyclo: ../Libraries/Third_Party/FatFs/src/%.c Libraries/Third_Party/FatFs/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U545xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I"/home/sharpe/Documents/Rowan/Rowan-Projects/Rowan-Rocketry/SRAD-Avionics/Ride-Along-Module/dev/ride-along-module/Libraries/Third_Party/FatFs/src" -I"/home/sharpe/Documents/Rowan/Rowan-Projects/Rowan-Rocketry/SRAD-Avionics/Ride-Along-Module/dev/ride-along-module/FATFS/App" -I"/home/sharpe/Documents/Rowan/Rowan-Projects/Rowan-Rocketry/SRAD-Avionics/Ride-Along-Module/dev/ride-along-module/FATFS/Target" -O0 -ffunction-sections -fdata-sections -Wall -Wmissing-include-dirs -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libraries-2f-Third_Party-2f-FatFs-2f-src

clean-Libraries-2f-Third_Party-2f-FatFs-2f-src:
	-$(RM) ./Libraries/Third_Party/FatFs/src/diskio.cyclo ./Libraries/Third_Party/FatFs/src/diskio.d ./Libraries/Third_Party/FatFs/src/diskio.o ./Libraries/Third_Party/FatFs/src/diskio.su ./Libraries/Third_Party/FatFs/src/ff.cyclo ./Libraries/Third_Party/FatFs/src/ff.d ./Libraries/Third_Party/FatFs/src/ff.o ./Libraries/Third_Party/FatFs/src/ff.su ./Libraries/Third_Party/FatFs/src/ff_gen_drv.cyclo ./Libraries/Third_Party/FatFs/src/ff_gen_drv.d ./Libraries/Third_Party/FatFs/src/ff_gen_drv.o ./Libraries/Third_Party/FatFs/src/ff_gen_drv.su

.PHONY: clean-Libraries-2f-Third_Party-2f-FatFs-2f-src

