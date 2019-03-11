################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Utils/PixelFormat.cpp 

OBJS += \
./src/Zuazo/Utils/PixelFormat.o 

CPP_DEPS += \
./src/Zuazo/Utils/PixelFormat.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Utils/%.o: ../src/Zuazo/Utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


