################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Media/Sources/FFmpeg.cpp \
../src/Zuazo/Media/Sources/V4L2.cpp 

OBJS += \
./src/Zuazo/Media/Sources/FFmpeg.o \
./src/Zuazo/Media/Sources/V4L2.o 

CPP_DEPS += \
./src/Zuazo/Media/Sources/FFmpeg.d \
./src/Zuazo/Media/Sources/V4L2.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Media/Sources/%.o: ../src/Zuazo/Media/Sources/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


