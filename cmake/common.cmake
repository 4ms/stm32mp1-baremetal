enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(ELF ${PROJECT_NAME}.elf)

# Directories with source files shared between projects
set(THIRDPARTY_DIR ../../third-party)
set(SHARED_DIR ../shared)
set(UBOOT_DIR ${THIRDPARTY_DIR}/u-boot/build)

# where uimg will be loaded and entered
set(LOADADDR 0xC2000040)
set(ENTRYPOINT 0xC2000040)

# Defines and flags
set(DEFAULT_DEFINES
	-DUSE_FULL_LL_DRIVER
	-DSTM32MP157Cxx
	-DSTM32MP1
	-DCORE_CA7
	)

set(MCU_FLAGS 
 	-mcpu=cortex-a7
	-march=armv7ve
	-mfpu=neon-vfpv4
	-mlittle-endian
	-mfloat-abi=hard
	)

set(DEFAULT_COMPILE_OPTIONS
	${MCU_FLAGS}
	-fno-common
	-fdata-sections
	-ffunction-sections
	-ffreestanding
	-nostartfiles
	-nostdlib
	-g3
	$<$<COMPILE_LANGUAGE:CXX>:@${PROJECT_SOURCE_DIR}/../../cmake/gcc-compile-options-cxx.txt>
	$<$<CONFIG:Debug>:-O0>
	$<$<CONFIG:Release>:-O3>

	)
