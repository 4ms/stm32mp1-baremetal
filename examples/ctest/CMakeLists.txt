cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/arm-none-eabi-gcc.cmake)
include(../../cmake/common.cmake)

project(ctest)

add_executable(${ELF}
	main.c
	startup.s
	${SHARED_DIR}/system/libc_stub.c
	${SHARED_DIR}/system/libcpp_stub.cc
	${SHARED_DIR}/system/system_ca7.c
	${SHARED_DIR}/system/mmu_ca7.c
	)

set(LINKSCRIPT ${PROJECT_SOURCE_DIR}/linkscript.ld)

target_include_directories(${ELF} PRIVATE
	${THIRDPARTY_DIR}/STM32MP1xx_HAL_Driver/Inc
	${THIRDPARTY_DIR}/CMSIS/Core_A/Include
	${THIRDPARTY_DIR}/CMSIS/Device/ST/STM32MP1xx/Include
	${SHARED_DIR}
	)

target_compile_definitions(${ELF} PRIVATE
	${DEFAULT_DEFINES}
	)

target_compile_options(${ELF} PRIVATE
	${MCU_FLAGS}
	-fno-common
	-fdata-sections
	-ffunction-sections
	-ffreestanding
	-nostartfiles
	-nostdlib
	-g2
	$<$<CONFIG:Debug>:-O0>
	$<$<CONFIG:Release>:-O3>
	$<$<COMPILE_LANGUAGE:CXX>:@${PROJECT_SOURCE_DIR}/../../cmake/gcc-compile-options-cxx.txt>
	)

target_link_options(${ELF} PRIVATE
	-Wl,--gc-sections 
	-Wl,-Map,${PROJECT_NAME}.map,--cref
	-Wl,--print-memory-usage
	${MCU_FLAGS}
	-T${LINKSCRIPT}
	-nostdlib  #Need to remove this for hal_lib project
	-nostartfiles
	-ffreestanding
	)

add_custom_command(TARGET ${ELF}
	POST_BUILD
	COMMAND arm-none-eabi-objcopy --output-target=ihex ${ELF} ${PROJECT_NAME}.hex
	COMMAND arm-none-eabi-objcopy -O binary ${ELF} ${PROJECT_NAME}.bin
 	COMMAND ${CMAKE_CURRENT_LIST_DIR}/${UBOOT_DIR}/tools/mkimage -A arm -C none -T kernel -a ${LOADADDR} -e ${ENTRYPOINT} -d ${PROJECT_NAME}.bin a7-main.uimg
	#DEPENDS mkimage
	)

add_custom_target(build-u-boot 
 	COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/../.. && scripts/build-u-boot.sh 
	)

# Additional files to remove on `make clean`
set_target_properties(${ELF} PROPERTIES ADDITIONAL_CLEAN_FILES
	"${PROJECT_NAME}.bin;${PROJECT_NAME}.hex;${PROJECT_NAME}.map;a7-main.uimg"
	)

# add_custom_command(TARGET ${ELF}
# 	POST_BUILD
# 	# DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${UBOOT_DIR}/tools/mkimage
# 	COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/../.. && scripts/build-u-boot.sh && ${CMAKE_CURRENT_LIST_DIR}/${UBOOT_DIR}/tools/mkimage -A arm -C none -T kernel -a ${LOADADDR} -e ${ENTRYPOINT} -d ${PROJECT_NAME}.bin a7-main.uimg
# 	)

# add_custom_command(
# 	OUTPUT ${CMAKE_CURRENT_LIST_DIR}/${UBOOT_DIR}/tools/mkimage
# 	)

## TODO: add these targets:
# install:
# 	cp $(UIMAGENAME) $(SDCARD_MOUNT_PATH)
# 	diskutil unmount $(SDCARD_MOUNT_PATH)

#################################################################

# Stuff that might come in handy as I learn cmake:

#LTO
# set_target_properties(${ELF} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)

# Set different flags for particular files (e.g. to disable warnings for library source files we can't modify)
# set_source_files_properties(
	# src/filename1.c
	# path/to/filename2.c
	# PROPERTIES
	# COMPILE_FLAGS -Wno-unused-parameter)

# Maybe we can search for the compiler? To support multiple versions installed
# find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
# find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ HINTS ${TOOLCHAIN_BIN_PATH})
# find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc HINTS ${TOOLCHAIN_BIN_PATH})

#${CMAKE_SOURCE_DIR} refers to the dir that cmake was first run from
#${PROJECT_SOURCE_DIR} refers to the dir that this CMakeLists.txt is in
