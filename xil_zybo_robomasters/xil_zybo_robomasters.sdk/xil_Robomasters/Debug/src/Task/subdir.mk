################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Task/CanBusTask.c \
../src/Task/ControlTask.c \
../src/Task/IOTask.c \
../src/Task/RemoteTask.c 

OBJS += \
./src/Task/CanBusTask.o \
./src/Task/ControlTask.o \
./src/Task/IOTask.o \
./src/Task/RemoteTask.o 

C_DEPS += \
./src/Task/CanBusTask.d \
./src/Task/ControlTask.d \
./src/Task/IOTask.d \
./src/Task/RemoteTask.d 


# Each subdirectory must supply rules for building sources it contributes
src/Task/%.o: ../src/Task/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../xil_Robomasters_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


