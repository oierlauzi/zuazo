################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Zuazo/Graphics/GL/FrameBuffer.cpp \
../src/Zuazo/Graphics/GL/Shader.cpp \
../src/Zuazo/Graphics/GL/Texture.cpp 

OBJS += \
./src/Zuazo/Graphics/GL/FrameBuffer.o \
./src/Zuazo/Graphics/GL/Shader.o \
./src/Zuazo/Graphics/GL/Texture.o 

CPP_DEPS += \
./src/Zuazo/Graphics/GL/FrameBuffer.d \
./src/Zuazo/Graphics/GL/Shader.d \
./src/Zuazo/Graphics/GL/Texture.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zuazo/Graphics/GL/%.o: ../src/Zuazo/Graphics/GL/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/oierlauzi/git/Zuazo/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


