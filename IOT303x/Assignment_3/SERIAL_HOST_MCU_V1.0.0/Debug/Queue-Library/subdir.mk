################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/Queue-Library/queue.c 

OBJS += \
./Queue-Library/queue.o 

C_DEPS += \
./Queue-Library/queue.d 


# Each subdirectory must supply rules for building sources it contributes
Queue-Library/queue.o: D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/Queue-Library/queue.c Queue-Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I../Inc -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Utilities" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Queue-2d-Library

clean-Queue-2d-Library:
	-$(RM) ./Queue-Library/queue.cyclo ./Queue-Library/queue.d ./Queue-Library/queue.o ./Queue-Library/queue.su

.PHONY: clean-Queue-2d-Library

