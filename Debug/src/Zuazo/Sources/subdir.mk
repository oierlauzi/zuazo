################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Sources/FFmpeg.cpp \
../src/Zuazo/Sources/SVG.cpp \
../src/Zuazo/Sources/V4L2.cpp 

OBJS += \
./src/Zuazo/Sources/FFmpeg.o \
./src/Zuazo/Sources/SVG.o \
./src/Zuazo/Sources/V4L2.o 

CPP_DEPS += \
./src/Zuazo/Sources/FFmpeg.d \
./src/Zuazo/Sources/SVG.d \
./src/Zuazo/Sources/V4L2.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Sources/%.o: ../src/Zuazo/Sources/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


