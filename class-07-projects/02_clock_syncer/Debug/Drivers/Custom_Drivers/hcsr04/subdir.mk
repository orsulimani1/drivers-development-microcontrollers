################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Custom_Drivers/hcsr04/hcsr04_driver.c 

OBJS += \
./Drivers/Custom_Drivers/hcsr04/hcsr04_driver.o 

C_DEPS += \
./Drivers/Custom_Drivers/hcsr04/hcsr04_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Custom_Drivers/hcsr04/%.o Drivers/Custom_Drivers/hcsr04/%.su Drivers/Custom_Drivers/hcsr04/%.cyclo: ../Drivers/Custom_Drivers/hcsr04/%.c Drivers/Custom_Drivers/hcsr04/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I"C:/src/drivers-development-microcontrollers/class-07-projects/02_clock_syncer/Drivers/Custom_Drivers/hcsr04" -I"C:/src/drivers-development-microcontrollers/class-07-projects/02_clock_syncer/Drivers/Custom_Drivers/hw512" -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Custom_Drivers/Inc -I../Drivers/Custom_Drivers/rtc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Custom_Drivers-2f-hcsr04

clean-Drivers-2f-Custom_Drivers-2f-hcsr04:
	-$(RM) ./Drivers/Custom_Drivers/hcsr04/hcsr04_driver.cyclo ./Drivers/Custom_Drivers/hcsr04/hcsr04_driver.d ./Drivers/Custom_Drivers/hcsr04/hcsr04_driver.o ./Drivers/Custom_Drivers/hcsr04/hcsr04_driver.su

.PHONY: clean-Drivers-2f-Custom_Drivers-2f-hcsr04

