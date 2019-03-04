################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Graphics/Context.cpp \
../src/Zuazo/Graphics/Frame.cpp \
../src/Zuazo/Graphics/Surface.cpp 

OBJS += \
./src/Zuazo/Graphics/Context.o \
./src/Zuazo/Graphics/Frame.o \
./src/Zuazo/Graphics/Surface.o 

CPP_DEPS += \
./src/Zuazo/Graphics/Context.d \
./src/Zuazo/Graphics/Frame.d \
./src/Zuazo/Graphics/Surface.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Graphics/%.o: ../src/Zuazo/Graphics/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


