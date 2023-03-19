################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Components/st25dv/st25dv.c \
../Core/Components/st25dv/st25dv_reg.c 

OBJS += \
./Core/Components/st25dv/st25dv.o \
./Core/Components/st25dv/st25dv_reg.o 

C_DEPS += \
./Core/Components/st25dv/st25dv.d \
./Core/Components/st25dv/st25dv_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Components/st25dv/%.o Core/Components/st25dv/%.su: ../Core/Components/st25dv/%.c Core/Components/st25dv/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Components-2f-st25dv

clean-Core-2f-Components-2f-st25dv:
	-$(RM) ./Core/Components/st25dv/st25dv.d ./Core/Components/st25dv/st25dv.o ./Core/Components/st25dv/st25dv.su ./Core/Components/st25dv/st25dv_reg.d ./Core/Components/st25dv/st25dv_reg.o ./Core/Components/st25dv/st25dv_reg.su

.PHONY: clean-Core-2f-Components-2f-st25dv

