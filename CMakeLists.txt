cmake_minimum_required(VERSION 3.10)

project(stm32mp1-baremetal)

# 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
add_definitions(-std=c++2a -Os)

add_definitions(-DUSE_HAL_DRIVER
                -DUSE_FULL_LL_DRIVER
                -DSTM32MP157Cxx
                -DSTM32MP1
                -DCORE_CA7)

add_definitions(-mfpu=neon-vfpv4
                -mfloat-abi=hard
                -mthumb-interwork
                -mtune=cortex-a7
                -mno-unaligned-access
                -funsafe-math-optimizations
                -mvectorize-with-neon-quad)   


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_HAL_DRIVER")

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third-party
    ${CMAKE_SOURCE_DIR}/third-party/STM32MP1xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/third-party/CMSIS/Core_A/Include
    ${CMAKE_SOURCE_DIR}/third-party/CMSIS/Device/ST/STM32MP1xx/Include)

if(BUILD_TARGET STREQUAL "mipi_dsi_lcd")
    include_directories(${CMAKE_SOURCE_DIR}/config/mipi_dsi_lcd)
elseif(BUILD_TARGET STREQUAL "audio_processor")
    include_directories(${CMAKE_SOURCE_DIR}/config/audio_processor)
endif()

#add_subdirectory(CoreModules)
add_subdirectory(drivers)
#add_subdirectory(stm32hal_patched)
#add_subdirectory(system)
#add_subdirectory(util)