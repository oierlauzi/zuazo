################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Timing.cpp \
../src/Zuazo/Zuazo.cpp \
../src/Zuazo/glad.cpp 

OBJS += \
./src/Zuazo/Timing.o \
./src/Zuazo/Zuazo.o \
./src/Zuazo/glad.o 

CPP_DEPS += \
./src/Zuazo/Timing.d \
./src/Zuazo/Zuazo.d \
./src/Zuazo/glad.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/%.o: ../src/Zuazo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


