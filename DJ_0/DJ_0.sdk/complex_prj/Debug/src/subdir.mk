################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/CanBusTask.c \
../src/ControlTask.c \
../src/IIC_API.c \
../src/IOTask.c \
../src/RemoteTask.c \
../src/Uart_0.c \
../src/bsp.c \
../src/imu.c \
../src/main.c \
../src/mpu6050_driver.c \
../src/pid.c \
../src/scutimer.c 

OBJS += \
./src/CanBusTask.o \
./src/ControlTask.o \
./src/IIC_API.o \
./src/IOTask.o \
./src/RemoteTask.o \
./src/Uart_0.o \
./src/bsp.o \
./src/imu.o \
./src/main.o \
./src/mpu6050_driver.o \
./src/pid.o \
./src/scutimer.o 

C_DEPS += \
./src/CanBusTask.d \
./src/ControlTask.d \
./src/IIC_API.d \
./src/IOTask.d \
./src/RemoteTask.d \
./src/Uart_0.d \
./src/bsp.d \
./src/imu.d \
./src/main.d \
./src/mpu6050_driver.d \
./src/pid.d \
./src/scutimer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../complex_prj_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


