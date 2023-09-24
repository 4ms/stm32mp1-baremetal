#
# Interface library for compile settings
#
add_library(mp15xa7_arch INTERFACE)
target_compile_definitions(
  mp15xa7_arch
  INTERFACE USE_HAL_DRIVER USE_FULL_LL_DRIVER STM32MP1 STM32MP157Cxx CORE_CA7
            $<$<BOOL:${USE_FEWER_MODULES}>:USE_FEWER_MODULES=1>)

# Note: MCU_FLAGS is used with linking as well
set(MCU_FLAGS
    -fno-math-errno
    -mcpu=cortex-a7
    -mlittle-endian
    -mfpu=neon-vfpv4
    -mfloat-abi=hard
    -mthumb-interwork
    -mno-unaligned-access
    -mtune=cortex-a7
    -mvectorize-with-neon-quad
    -funsafe-math-optimizations)

target_compile_features(mp15xa7_arch INTERFACE cxx_std_23)

target_compile_options(
  mp15xa7_arch
  INTERFACE ${MCU_FLAGS}
            "SHELL:--param l1-cache-size=32"
            "SHELL:--param l1-cache-line-size=64"
            "SHELL:--param l2-cache-size=256"
            $<$<CONFIG:Debug>:-O0
            -g3>
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:RelWithDebInfo>:-O3
            -g3>
            -fno-common
            -fdata-sections
            -ffunction-sections
            -nostdlib
            -nostartfiles
            -ffreestanding
            -fno-unwind-tables
            -Wall
            -Werror=return-type
            -Wsign-compare
            $<$<COMPILE_LANGUAGE:CXX>:
            # -flto=auto
            -ffold-simple-inlines
            -Wno-psabi
            -fno-rtti
            -fno-exceptions
            -Wno-register
            -Wno-volatile
            -fno-threadsafe-statics
            >)

target_link_options(mp15xa7_arch INTERFACE ${MCU_FLAGS})
