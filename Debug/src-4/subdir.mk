################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src-4/control_functions.c \
../src-4/display_functions.c \
../src-4/gestion_fichiers.c \
../src-4/main.c \
../src-4/model_functions.c 

C_DEPS += \
./src-4/control_functions.d \
./src-4/display_functions.d \
./src-4/gestion_fichiers.d \
./src-4/main.d \
./src-4/model_functions.d 

OBJS += \
./src-4/control_functions.o \
./src-4/display_functions.o \
./src-4/gestion_fichiers.o \
./src-4/main.o \
./src-4/model_functions.o 


# Each subdirectory must supply rules for building sources it contributes
src-4/%.o: ../src-4/%.c src-4/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/homebrew/Cellar/ncurses/6.5/include -I/opt/homebrew/Cellar/ncurses/6.5/include/ncursesw -O0 -g3 -Wall -c -fmessage-length=0 -arch arm64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2d-4

clean-src-2d-4:
	-$(RM) ./src-4/control_functions.d ./src-4/control_functions.o ./src-4/display_functions.d ./src-4/display_functions.o ./src-4/gestion_fichiers.d ./src-4/gestion_fichiers.o ./src-4/main.d ./src-4/main.o ./src-4/model_functions.d ./src-4/model_functions.o

.PHONY: clean-src-2d-4

