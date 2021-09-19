cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
project(stm32mp1-baremetal)

add_subdirectory(examples/ctest)
