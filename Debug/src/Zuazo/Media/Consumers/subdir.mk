################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Media/Consumers/Window.cpp 

OBJS += \
./src/Zuazo/Media/Consumers/Window.o 

CPP_DEPS += \
./src/Zuazo/Media/Consumers/Window.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Media/Consumers/%.o: ../src/Zuazo/Media/Consumers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


