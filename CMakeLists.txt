cmake_minimum_required(VERSION 3.15.3)

project(stm32mp1-baremetal)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Shared directories
set(EXTLIB_DIR third-party)
set(SHARED_DIR examples/shared)

#ctest project
set(EXAMPLE_DIR examples/ctest)
set(SOURCES
	${EXAMPLE_DIR}/main.c
	${EXAMPLE_DIR}/startup.s
	${SHARED_DIR}/system/libc_stub.c
	${SHARED_DIR}/system/libcpp_stub.cc
	${SHARED_DIR}/system/system_ca7.c
	${SHARED_DIR}/system/mmu_ca7.c
	)
set(LINKSCRIPT ${PROJECT_SOURCE_DIR}/${EXAMPLE_DIR}/linkscript.ld)


set(EXECUTABLE ${PROJECT_NAME}.elf)

set(LOADADDR 0xC2000040)

add_executable(${EXECUTABLE} ${SOURCES})

target_compile_definitions(${EXECUTABLE} PRIVATE
	-DUSE_FULL_LL_DRIVER
	-DSTM32MP157Cxx
	-DSTM32MP1
	-DCORE_CA7
	)

target_include_directories(${EXECUTABLE} PRIVATE
	${EXTLIB_DIR}/STM32MP1xx_HAL_Driver/Inc
	${EXTLIB_DIR}/CMSIS/Core_A/Include
	${EXTLIB_DIR}/CMSIS/Device/ST/STM32MP1xx/Include
	${SHARED_DIR}
	)


set(MCU_FLAGS 
 	-mcpu=cortex-a7
	-march=armv7ve
	-mfpu=neon-vfpv4
	-mlittle-endian
	-mfloat-abi=hard
	)

target_compile_options(${EXECUTABLE} PRIVATE
	${MCU_FLAGS}
	-fno-common
	-fdata-sections
	-ffunction-sections
	-ffreestanding
	-nostartfiles
	-nostdlib
	-Og
	$<$<COMPILE_LANGUAGE:CXX>:@${CMAKE_SOURCE_DIR}/cmake/gcc-compile-options-cxx.txt>
	$<$<CONFIG:Debug>:-O0>
	$<$<CONFIG:Release>:-O3>
	)

target_link_options(${EXECUTABLE} PRIVATE
	-Wl,--gc-sections 
	-Wl,-Map,${PROJECT_NAME}.map,--cref
	${MCU_FLAGS}
	-T${LINKSCRIPT}
	-nostdlib  #Need to remove this for hal_lib project
	-nostartfiles
	-ffreestanding
	)

# Set different flags for particular files (e.g. to disable warnings for library source files we can't modify)
# set_source_files_properties(
	# BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
	# BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
	# BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
	# PROPERTIES
	# COMPILE_FLAGS -Wno-unused-parameter)

add_custom_command(TARGET ${EXECUTABLE}
	POST_BUILD
	COMMAND arm-none-eabi-objcopy --output-target=ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
	COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
	)

# install:
# 	cp $(UIMAGENAME) $(SDCARD_MOUNT_PATH)
# 	diskutil unmount $(SDCARD_MOUNT_PATH)

# $(UIMAGENAME): $(BIN) $(UBOOTDIR)/tools/mkimage
# 	$(info Creating uimg file)
# 	@$(UBOOTDIR)/tools/mkimage -A arm -C none -T kernel -a $(LOADADDR) -e $(ENTRYPOINT) -d $< $@

# $(UBOOTDIR)/tools/mkimage:
# 	$(info Building U-boot bootloader)
# 	@cd ../.. && scripts/build-u-boot.sh

# find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
# find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ HINTS ${TOOLCHAIN_BIN_PATH})
# find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc HINTS ${TOOLCHAIN_BIN_PATH})

