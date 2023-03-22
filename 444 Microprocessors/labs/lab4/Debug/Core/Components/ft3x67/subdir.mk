################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Components/ft3x67/ft3x67.c 

OBJS += \
./Core/Components/ft3x67/ft3x67.o 

C_DEPS += \
./Core/Components/ft3x67/ft3x67.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Components/ft3x67/%.o Core/Components/ft3x67/%.su: ../Core/Components/ft3x67/%.c Core/Components/ft3x67/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Components-2f-ft3x67

clean-Core-2f-Components-2f-ft3x67:
	-$(RM) ./Core/Components/ft3x67/ft3x67.d ./Core/Components/ft3x67/ft3x67.o ./Core/Components/ft3x67/ft3x67.su

.PHONY: clean-Core-2f-Components-2f-ft3x67
