################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/preprocessing/GaussianSPDenoise/denoise.cpp 

OBJS += \
./src/preprocessing/GaussianSPDenoise/denoise.o 

CPP_DEPS += \
./src/preprocessing/GaussianSPDenoise/denoise.d 


# Each subdirectory must supply rules for building sources it contributes
src/preprocessing/GaussianSPDenoise/%.o: ../src/preprocessing/GaussianSPDenoise/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/ubuntu/opencv/include -I/home/ubuntu/opencv/include/opencv -I/home/ubuntu/tesseract/include -I/home/ubuntu/leptonica/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


