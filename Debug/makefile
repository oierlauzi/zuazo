################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/Zuazo/Utils/subdir.mk
-include src/Zuazo/Updateables/subdir.mk
-include src/Zuazo/Sources/subdir.mk
-include src/Zuazo/Processors/subdir.mk
-include src/Zuazo/Graphics/GL/subdir.mk
-include src/Zuazo/Graphics/subdir.mk
-include src/Zuazo/Consumers/subdir.mk
-include src/Zuazo/subdir.mk
-include src/subdir.mk
-include build/Zuazo/Updateables/subdir.mk
-include build/Zuazo/Sources/subdir.mk
-include build/Zuazo/Processors/subdir.mk
-include build/Zuazo/Graphics/GL/subdir.mk
-include build/Zuazo/Graphics/subdir.mk
-include build/Zuazo/Consumers/subdir.mk
-include build/Zuazo/subdir.mk
-include build/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libZuazo.so

# Tool invocations
libZuazo.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -shared -o "libZuazo.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(CC_DEPS)$(C++_DEPS)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) libZuazo.so
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
