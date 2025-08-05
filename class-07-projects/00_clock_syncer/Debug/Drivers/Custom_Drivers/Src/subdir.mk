################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Custom_Drivers/Src/rtc_driver.c 

OBJS += \
./Drivers/Custom_Drivers/Src/rtc_driver.o 

C_DEPS += \
./Drivers/Custom_Drivers/Src/rtc_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Custom_Drivers/Src/%.o Drivers/Custom_Drivers/Src/%.su Drivers/Custom_Drivers/Src/%.cyclo: ../Drivers/Custom_Drivers/Src/%.c Drivers/Custom_Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Custom_Drivers/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Custom_Drivers-2f-Src

clean-Drivers-2f-Custom_Drivers-2f-Src:
	-$(RM) ./Drivers/Custom_Drivers/Src/rtc_driver.cyclo ./Drivers/Custom_Drivers/Src/rtc_driver.d ./Drivers/Custom_Drivers/Src/rtc_driver.o ./Drivers/Custom_Drivers/Src/rtc_driver.su

.PHONY: clean-Drivers-2f-Custom_Drivers-2f-Src

