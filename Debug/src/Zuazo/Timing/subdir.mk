################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Timing/TimeInterval.cpp \
../src/Zuazo/Timing/Timing.cpp \
../src/Zuazo/Timing/TimingTable.cpp 

OBJS += \
./src/Zuazo/Timing/TimeInterval.o \
./src/Zuazo/Timing/Timing.o \
./src/Zuazo/Timing/TimingTable.o 

CPP_DEPS += \
./src/Zuazo/Timing/TimeInterval.d \
./src/Zuazo/Timing/Timing.d \
./src/Zuazo/Timing/TimingTable.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Timing/%.o: ../src/Zuazo/Timing/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


