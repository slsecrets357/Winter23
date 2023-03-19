################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Components/lsm6dsl/lsm6dsl.c 

OBJS += \
./Core/Components/lsm6dsl/lsm6dsl.o 

C_DEPS += \
./Core/Components/lsm6dsl/lsm6dsl.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Components/lsm6dsl/%.o Core/Components/lsm6dsl/%.su: ../Core/Components/lsm6dsl/%.c Core/Components/lsm6dsl/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Components-2f-lsm6dsl

clean-Core-2f-Components-2f-lsm6dsl:
	-$(RM) ./Core/Components/lsm6dsl/lsm6dsl.d ./Core/Components/lsm6dsl/lsm6dsl.o ./Core/Components/lsm6dsl/lsm6dsl.su

.PHONY: clean-Core-2f-Components-2f-lsm6dsl

