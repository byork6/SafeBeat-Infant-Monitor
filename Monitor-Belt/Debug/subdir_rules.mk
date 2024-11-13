################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none -mlittle-endian -mthumb -Oz -I"C:/Users/byork/workspace_ccstheia/Monitor-Belt" -I"C:/Users/byork/workspace_ccstheia/Monitor-Belt/Debug" -I"/source" -I"/kernel/tirtos7/packages" -I"/source/ti/posix/ticlang" -gdwarf-3 -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/byork/workspace_ccstheia/Monitor-Belt/Debug/syscfg" -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-641631409: ../main.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccstheia151/ccs/utils/sysconfig_1.21.1/sysconfig_cli.bat" --script "C:/Users/byork/workspace_ccstheia/Monitor-Belt/main.syscfg" -o "syscfg" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/error.h: build-641631409 ../main.syscfg
syscfg: build-641631409


