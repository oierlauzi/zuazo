################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Updateables/NonLinear.cpp \
../src/Zuazo/Updateables/Updateable.cpp 

OBJS += \
./src/Zuazo/Updateables/NonLinear.o \
./src/Zuazo/Updateables/Updateable.o 

CPP_DEPS += \
./src/Zuazo/Updateables/NonLinear.d \
./src/Zuazo/Updateables/Updateable.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Updateables/%.o: ../src/Zuazo/Updateables/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


