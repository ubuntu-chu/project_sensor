################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include reactor/subdir.mk
-include obj/subdir.mk
-include lib/common/subdir.mk
-include includes/subdir.mk
-include bsp/hal/subdir.mk
-include bsp/driver/uart/subdir.mk
-include bsp/driver/timer/subdir.mk
-include bsp/driver/template/subdir.mk
-include bsp/driver/storage/subdir.mk
-include bsp/driver/pwm/subdir.mk
-include bsp/driver/pin/subdir.mk
-include bsp/driver/event/subdir.mk
-include bsp/driver/commu/subdir.mk
-include bsp/driver/ad/subdir.mk
-include bsp/driver/subdir.mk
-include bsp/cpu/subdir.mk
-include bsp/subdir.mk
-include app/subdir.mk
-include api/log/subdir.mk
-include api/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: project_sensor

# Tool invocations
project_sensor: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++  -o "project_sensor" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) project_sensor
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
