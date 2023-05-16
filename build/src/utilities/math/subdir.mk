################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/utilities/math/2dGeometry.cpp \
../src/utilities/math/matrix.cpp \
../src/utilities/math/randomGenerator.cpp 

OBJS += \
./src/utilities/math/2dGeometry.o \
./src/utilities/math/matrix.o \
./src/utilities/math/randomGenerator.o 

CPP_DEPS += \
./src/utilities/math/2dGeometry.d \
./src/utilities/math/matrix.d \
./src/utilities/math/randomGenerator.d 


# Each subdirectory must supply rules for building sources it contributes
src/utilities/math/%.o: ../src/utilities/math/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include -I$(MATLAB_INCLUDE_PATH) -O3 -Wall -Wextra -c -fmessage-length=0 -DMATLAB_DEFAULT_RELEASE=R2017b  -DUSE_MEX_CMD   -D_GNU_SOURCE -DMATLAB_MEX_FILE -fexceptions -fno-omit-frame-pointer  -std=c++11 -fwrapv -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


