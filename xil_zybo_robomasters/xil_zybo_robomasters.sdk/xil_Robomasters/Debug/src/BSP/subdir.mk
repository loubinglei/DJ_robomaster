################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BSP/GIC_init.c \
../src/BSP/IIC_API.c \
../src/BSP/Uart_0.c \
../src/BSP/bsp.c \
../src/BSP/can_0.c \
../src/BSP/can_1.c \
../src/BSP/scutimer.c 

OBJS += \
./src/BSP/GIC_init.o \
./src/BSP/IIC_API.o \
./src/BSP/Uart_0.o \
./src/BSP/bsp.o \
./src/BSP/can_0.o \
./src/BSP/can_1.o \
./src/BSP/scutimer.o 

C_DEPS += \
./src/BSP/GIC_init.d \
./src/BSP/IIC_API.d \
./src/BSP/Uart_0.d \
./src/BSP/bsp.d \
./src/BSP/can_0.d \
./src/BSP/can_1.d \
./src/BSP/scutimer.d 


# Each subdirectory must supply rules for building sources it contributes
src/BSP/%.o: ../src/BSP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../xil_Robomasters_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


