################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/textExtraction/main.cpp 

OBJS += \
./src/textExtraction/main.o 

CPP_DEPS += \
./src/textExtraction/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/textExtraction/%.o: ../src/textExtraction/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -DGSL_DLL -D_GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I/usr/include -I/home/yulin/gsl/include -I/usr/local/include/opencv -I/home/yulin/ocr/tesseract/include -I/usr/include/libxml2 -I/usr/include/armadillo_bits -I/usr/local/include/mlpack -I/usr/lib/jvm/java-8-oracle/include/linux -I/usr/lib/jvm/java-8-oracle/include -O0 -g3 -D_OCRUS_DEBUG -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


