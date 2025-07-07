################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/Src/gpio_driver.c \
../drivers/Src/i2c_common_driver.c \
../drivers/Src/i2c_interrupt_driver.c \
../drivers/Src/i2c_polling_driver.c 

OBJS += \
./drivers/Src/gpio_driver.o \
./drivers/Src/i2c_common_driver.o \
./drivers/Src/i2c_interrupt_driver.o \
./drivers/Src/i2c_polling_driver.o 

C_DEPS += \
./drivers/Src/gpio_driver.d \
./drivers/Src/i2c_common_driver.d \
./drivers/Src/i2c_interrupt_driver.d \
./drivers/Src/i2c_polling_driver.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/Src/%.o drivers/Src/%.su drivers/Src/%.cyclo: ../drivers/Src/%.c drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411VETx -DSTM32 -DSTM32F4 -DSTM32F411E_DISCO -c -I../Inc -I"../drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-drivers-2f-Src

clean-drivers-2f-Src:
	-$(RM) ./drivers/Src/gpio_driver.cyclo ./drivers/Src/gpio_driver.d ./drivers/Src/gpio_driver.o ./drivers/Src/gpio_driver.su ./drivers/Src/i2c_common_driver.cyclo ./drivers/Src/i2c_common_driver.d ./drivers/Src/i2c_common_driver.o ./drivers/Src/i2c_common_driver.su ./drivers/Src/i2c_interrupt_driver.cyclo ./drivers/Src/i2c_interrupt_driver.d ./drivers/Src/i2c_interrupt_driver.o ./drivers/Src/i2c_interrupt_driver.su ./drivers/Src/i2c_polling_driver.cyclo ./drivers/Src/i2c_polling_driver.d ./drivers/Src/i2c_polling_driver.o ./drivers/Src/i2c_polling_driver.su

.PHONY: clean-drivers-2f-Src

