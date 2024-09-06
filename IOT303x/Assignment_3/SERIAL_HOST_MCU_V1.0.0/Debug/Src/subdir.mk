################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main_Asm3_IOT303.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/main_Asm3_IOT303.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/main_Asm3_IOT303.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I../Inc -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib" -I"D:/1_Studying/1_IoT/1_FUNIX/Libraries_Add/SDK_1.0.3_NUCLEO-F401RE/shared/Utilities" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/main_Asm3_IOT303.cyclo ./Src/main_Asm3_IOT303.d ./Src/main_Asm3_IOT303.o ./Src/main_Asm3_IOT303.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

