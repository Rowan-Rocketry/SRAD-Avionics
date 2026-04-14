################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
F:/STM32CubeIDE/STM32Cube_FW_WL_V1.4.0/Projects/NUCLEO-WL55JC/Applications/SubGHz_Phy/SubGHz_Phy_Per/Core/Src/system_stm32wlxx.c 

OBJS += \
./Drivers/CMSIS/system_stm32wlxx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32wlxx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32wlxx.o: F:/STM32CubeIDE/STM32Cube_FW_WL_V1.4.0/Projects/NUCLEO-WL55JC/Applications/SubGHz_Phy/SubGHz_Phy_Per/Core/Src/system_stm32wlxx.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../Core/Inc -I../../SubGHz_Phy/App -I../../SubGHz_Phy/Target -I../../../../../../../Utilities/trace/adv_trace -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Utilities/misc -I../../../../../../../Utilities/sequencer -I../../../../../../../Utilities/timer -I../../../../../../../Utilities/lpm/tiny_lpm -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Middlewares/Third_Party/SubGHz_Phy -I../../../../../../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Utilities/trace/adv_trace -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Drivers/STM32WLxx_HAL_Driver/Inc -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Utilities/misc -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Utilities/sequencer -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Utilities/timer -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Utilities/lpm/tiny_lpm -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Drivers/CMSIS/Device/ST/STM32WLxx/Include -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Middlewares/Third_Party/SubGHz_Phy/radio_driver -IF:/STM32Cube/Repository/STM32Cube_FW_WL_V1.5.0/Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS

clean-Drivers-2f-CMSIS:
	-$(RM) ./Drivers/CMSIS/system_stm32wlxx.cyclo ./Drivers/CMSIS/system_stm32wlxx.d ./Drivers/CMSIS/system_stm32wlxx.o ./Drivers/CMSIS/system_stm32wlxx.su

.PHONY: clean-Drivers-2f-CMSIS

