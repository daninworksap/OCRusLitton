################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/preprocessing/noiseLevel/noiseLevel.cpp 

OBJS += \
./src/preprocessing/noiseLevel/noiseLevel.o 

CPP_DEPS += \
./src/preprocessing/noiseLevel/noiseLevel.d 


# Each subdirectory must supply rules for building sources it contributes
src/preprocessing/noiseLevel/%.o: ../src/preprocessing/noiseLevel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -DGSL_DLL -D_GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I/usr/include -I/home/yulin/gsl/include -I/usr/local/include/opencv -I/home/yulin/ocr/tesseract/include -I/usr/include/libxml2 -I/usr/include/armadillo_bits -I/usr/local/include/mlpack -I/usr/lib/jvm/java-8-oracle/include/linux -I/usr/lib/jvm/java-8-oracle/include -O0 -g3 -D_OCRUS_DEBUG -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


