################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../cc13x1_cc26x1_tirtos7.cmd 

SYSCFG_SRCS += \
../main.syscfg 

C_SRCS += \
../main.c \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c \
./syscfg/ti_sysbios_config.c \
../main_tirtos.c 

GEN_FILES += \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c \
./syscfg/ti_utils_build_compiler.opt \
./syscfg/ti_sysbios_config.c 

GEN_MISC_DIRS += \
./syscfg 

C_DEPS += \
./main.d \
./syscfg/ti_devices_config.d \
./syscfg/ti_drivers_config.d \
./syscfg/ti_sysbios_config.d \
./main_tirtos.d 

GEN_OPTS += \
./syscfg/ti_utils_build_compiler.opt 

OBJS += \
./main.o \
./syscfg/ti_devices_config.o \
./syscfg/ti_drivers_config.o \
./syscfg/ti_sysbios_config.o \
./main_tirtos.o 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.genlibs \
./syscfg/syscfg_c.rov.xs \
./syscfg/ti_sysbios_config.h 

GEN_MISC_DIRS__QUOTED += \
"syscfg" 

OBJS__QUOTED += \
"main.o" \
"syscfg\ti_devices_config.o" \
"syscfg\ti_drivers_config.o" \
"syscfg\ti_sysbios_config.o" \
"main_tirtos.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\ti_utils_build_linker.cmd.genlibs" \
"syscfg\syscfg_c.rov.xs" \
"syscfg\ti_sysbios_config.h" 

C_DEPS__QUOTED += \
"main.d" \
"syscfg\ti_devices_config.d" \
"syscfg\ti_drivers_config.d" \
"syscfg\ti_sysbios_config.d" \
"main_tirtos.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_devices_config.c" \
"syscfg\ti_drivers_config.c" \
"syscfg\ti_utils_build_compiler.opt" \
"syscfg\ti_sysbios_config.c" 

C_SRCS__QUOTED += \
"../main.c" \
"./syscfg/ti_devices_config.c" \
"./syscfg/ti_drivers_config.c" \
"./syscfg/ti_sysbios_config.c" \
"../main_tirtos.c" 

SYSCFG_SRCS__QUOTED += \
"../main.syscfg" 


