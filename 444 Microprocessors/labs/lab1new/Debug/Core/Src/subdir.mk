################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Src/kalmanfilter_asm.s \
../Core/Src/kalmanfilter_asm2.s 

C_SRCS += \
../Core/Src/Kalmanfilter_DSP.c \
../Core/Src/kalmanfilter.c \
../Core/Src/kf_asm.c \
../Core/Src/main.c \
../Core/Src/statistics.c \
../Core/Src/stddev.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c 

OBJS += \
./Core/Src/Kalmanfilter_DSP.o \
./Core/Src/kalmanfilter.o \
./Core/Src/kalmanfilter_asm.o \
./Core/Src/kalmanfilter_asm2.o \
./Core/Src/kf_asm.o \
./Core/Src/main.o \
./Core/Src/statistics.o \
./Core/Src/stddev.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o 

S_DEPS += \
./Core/Src/kalmanfilter_asm.d \
./Core/Src/kalmanfilter_asm2.d 

C_DEPS += \
./Core/Src/Kalmanfilter_DSP.d \
./Core/Src/kalmanfilter.d \
./Core/Src/kf_asm.d \
./Core/Src/main.d \
./Core/Src/statistics.d \
./Core/Src/stddev.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o: ../Core/Src/%.s Core/Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Kalmanfilter_DSP.d ./Core/Src/Kalmanfilter_DSP.o ./Core/Src/Kalmanfilter_DSP.su ./Core/Src/kalmanfilter.d ./Core/Src/kalmanfilter.o ./Core/Src/kalmanfilter.su ./Core/Src/kalmanfilter_asm.d ./Core/Src/kalmanfilter_asm.o ./Core/Src/kalmanfilter_asm2.d ./Core/Src/kalmanfilter_asm2.o ./Core/Src/kf_asm.d ./Core/Src/kf_asm.o ./Core/Src/kf_asm.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/statistics.d ./Core/Src/statistics.o ./Core/Src/statistics.su ./Core/Src/stddev.d ./Core/Src/stddev.o ./Core/Src/stddev.su ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su

.PHONY: clean-Core-2f-Src

