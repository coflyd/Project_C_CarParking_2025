################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/control_functions.c \
../src/display_functions.c \
../src/gestion_fichiers.c \
../src/main.c \
../src/model_functions.c 

C_DEPS += \
./src/control_functions.d \
./src/display_functions.d \
./src/gestion_fichiers.d \
./src/main.d \
./src/model_functions.d 

OBJS += \
./src/control_functions.o \
./src/display_functions.o \
./src/gestion_fichiers.o \
./src/main.o \
./src/model_functions.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/homebrew/Cellar/ncurses/6.5/include -I/opt/homebrew/Cellar/ncurses/6.5/include/ncursesw -O0 -g3 -Wall -c -fmessage-length=0 -arch arm64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/control_functions.d ./src/control_functions.o ./src/display_functions.d ./src/display_functions.o ./src/gestion_fichiers.d ./src/gestion_fichiers.o ./src/main.d ./src/main.o ./src/model_functions.d ./src/model_functions.o

.PHONY: clean-src

