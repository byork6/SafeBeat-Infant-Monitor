################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none -mlittle-endian -mthumb -Oz -I"C:/Users/byork/workspace_ccstheia/Control-Box" -I"C:/Users/byork/workspace_ccstheia/Control-Box/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_7_41_00_17/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_7_41_00_17/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_7_41_00_17/source/ti/posix/ticlang" -gdwarf-3 -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/byork/workspace_ccstheia/Control-Box/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1785773631: ../main.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1.18.1/sysconfig_cli.bat" --script "C:/Users/byork/workspace_ccstheia/Control-Box/main.syscfg" -o "syscfg" -s "C:/ti/simplelink_cc13xx_cc26xx_sdk_7_41_00_17/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_devices_config.c: build-1785773631 ../main.syscfg
syscfg/ti_drivers_config.c: build-1785773631
syscfg/ti_drivers_config.h: build-1785773631
syscfg/ti_utils_build_linker.cmd.genlibs: build-1785773631
syscfg/ti_utils_build_compiler.opt: build-1785773631
syscfg/syscfg_c.rov.xs: build-1785773631
syscfg/ti_sysbios_config.h: build-1785773631
syscfg/ti_sysbios_config.c: build-1785773631
syscfg: build-1785773631

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=none -mlittle-endian -mthumb -Oz -I"C:/Users/byork/workspace_ccstheia/Control-Box" -I"C:/Users/byork/workspace_ccstheia/Control-Box/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_7_41_00_17/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_7_41_00_17/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_7_41_00_17/source/ti/posix/ticlang" -gdwarf-3 -march=armv7e-m -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/byork/workspace_ccstheia/Control-Box/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


