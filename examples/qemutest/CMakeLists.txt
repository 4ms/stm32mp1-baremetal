cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(c4core-quickstart LANGUAGES CXX)
set(TOOLCHAIN_PREFIX "/Users/design/4ms/stm32/gcc-arm-none-eabi-10.3-2021.07/")
add_executable(c4core-quickstart startup.s main.cpp)
set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/linkscript.ld")
add_custom_command(
  TARGET c4core-quickstart
  POST_BUILD
  COMMAND arm-none-eabi-objcopy -O binary c4core-quickstart c4core-quickstart.bin
)
