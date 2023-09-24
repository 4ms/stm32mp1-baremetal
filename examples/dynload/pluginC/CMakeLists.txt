cmake_minimum_required(VERSION 3.22)
set(CMAKE_TOOLCHAIN_FILE
    ${CMAKE_CURRENT_LIST_DIR}/../cmake/arm-none-eabi-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_BUILD_TYPE "RelWithDebInfo")
set(CMAKE_BUILD_TYPE "Release")

include(${CMAKE_SOURCE_DIR}/../cmake/arch_mp15xa7.cmake)
include(${CMAKE_SOURCE_DIR}/../cmake/common.cmake)

project(
  PluginC
  VERSION 0.1
  DESCRIPTION "PluginC for stm32mp1-baremetal dynload example"
  LANGUAGES C CXX ASM)

add_executable(pluginC.elf pluginC.cc calc.cc init.cc)
# ../../../shared/system/libc_stub.c ../../../shared/system/libcpp_stub.cc)

# target_include_directories(pluginC PRIVATE ../)

target_link_libraries(pluginC.elf PUBLIC mp15xa7_arch)

target_compile_options(pluginC.elf PUBLIC -fPIC -ffreestanding -nostdlib
                                          -nostartfiles)
target_link_script(
  pluginC
  ${CMAKE_CURRENT_LIST_DIR}/linkscript.ld
  -Wl,--gc-sections
  -Wno-psabi
  -ffreestanding
  -nostartfiles
  -nostdlib
  -fPIC)

add_bin_hex_command(pluginC)

# target_link_options(pluginC PRIVATE ${MCU_FLAGS})
target_link_options(pluginC.elf PRIVATE ${MCU_FLAGS})

# target_compile_features(pluginC PRIVATE cxx_std_23)
