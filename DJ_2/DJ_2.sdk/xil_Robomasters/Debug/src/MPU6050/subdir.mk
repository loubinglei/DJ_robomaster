################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MPU6050/imu.c \
../src/MPU6050/mpu6050_driver.c 

OBJS += \
./src/MPU6050/imu.o \
./src/MPU6050/mpu6050_driver.o 

C_DEPS += \
./src/MPU6050/imu.d \
./src/MPU6050/mpu6050_driver.d 


# Each subdirectory must supply rules for building sources it contributes
src/MPU6050/%.o: ../src/MPU6050/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../xil_Robomasters_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


