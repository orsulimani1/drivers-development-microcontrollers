################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/scheduler/scheduler.c 

OBJS += \
./Core/scheduler/scheduler.o 

C_DEPS += \
./Core/scheduler/scheduler.d 


# Each subdirectory must supply rules for building sources it contributes
Core/scheduler/%.o Core/scheduler/%.su Core/scheduler/%.cyclo: ../Core/scheduler/%.c Core/scheduler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Custom_Drivers/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-scheduler

clean-Core-2f-scheduler:
	-$(RM) ./Core/scheduler/scheduler.cyclo ./Core/scheduler/scheduler.d ./Core/scheduler/scheduler.o ./Core/scheduler/scheduler.su

.PHONY: clean-Core-2f-scheduler

