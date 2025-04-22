################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/Kalman_filter/kalman_filter.c 

OBJS += \
./Kalman_filter/kalman_filter.o 

C_DEPS += \
./Kalman_filter/kalman_filter.d 


# Each subdirectory must supply rules for building sources it contributes
Kalman_filter/kalman_filter.o: D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/Kalman_filter/kalman_filter.c Kalman_filter/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I../Inc -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/Kalman_filter" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Utilities" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Kalman_filter

clean-Kalman_filter:
	-$(RM) ./Kalman_filter/kalman_filter.cyclo ./Kalman_filter/kalman_filter.d ./Kalman_filter/kalman_filter.o ./Kalman_filter/kalman_filter.su

.PHONY: clean-Kalman_filter

