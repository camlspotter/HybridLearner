################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/simulation/simulation.cpp \
../src/simulation/simulation_trace_testing.cpp 

OBJS += \
./src/simulation/simulation.o \
./src/simulation/simulation_trace_testing.o 

CPP_DEPS += \
./src/simulation/simulation.d \
./src/simulation/simulation_trace_testing.d 


# Each subdirectory must supply rules for building sources it contributes
src/simulation/%.o: ../src/simulation/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include -I$(MATLAB_INCLUDE_PATH) -O3 -Wall -Wextra -c -fmessage-length=0 -DMATLAB_DEFAULT_RELEASE=R2017b  -DUSE_MEX_CMD   -D_GNU_SOURCE -DMATLAB_MEX_FILE -fexceptions -fno-omit-frame-pointer  -std=c++11 -fwrapv -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


