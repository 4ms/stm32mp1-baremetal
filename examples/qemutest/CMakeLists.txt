cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(c4core-quickstart LANGUAGES CXX ASM)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/linkscript.ld")

set(OBJECT_GEN_FLAGS "-g3 -mcpu=cortex-a9 -mlittle-endian")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OBJECT_GEN_FLAGS}" CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OBJECT_GEN_FLAGS}" CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${OBJECT_GEN_FLAGS}" CACHE INTERNAL "ASM Compiler options")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT}" CACHE INTERNAL "Linker options")

add_executable(c4core-quickstart.elf startup.s main.cpp)
add_custom_command(
  TARGET c4core-quickstart.elf
  POST_BUILD
  COMMAND arm-none-eabi-objcopy -O binary c4core-quickstart.elf c4core-quickstart.bin
)
