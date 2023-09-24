function(target_link_script target_base link_script)
  target_link_options(
    ${target_base}.elf
    PRIVATE
    -Wl,-Map,$<TARGET_FILE_DIR:${target_base}.elf>/${target_base}.map,--cref
    -Wl,--no-warn-rwx-segments
    -T
    ${link_script}
    ${ARGN}
  )
  set_target_properties(${target_base}.elf PROPERTIES LINK_DEPENDS ${link_script})
endfunction()

function(add_bin_hex_command target_base)
  set(BASENAME $<TARGET_FILE_DIR:${target_base}.elf>/${target_base})
  add_custom_command(
    TARGET ${target_base}.elf
    POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE:${target_base}.elf> ${BASENAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE:${target_base}.elf> ${BASENAME}.bin
    COMMAND arm-none-eabi-size -d $<TARGET_FILE:${target_base}.elf>
    COMMENT "Built .elf file: $<TARGET_FILE:${target_base}.elf>"
  )
  set_target_properties(
    ${target_base}.elf PROPERTIES ADDITIONAL_CLEAN_FILES "${BASE_NAME}.hex;${BASE_NAME}.bin;${BASE_NAME}.map"
  )
endfunction()

set(CMAKE_VERBOSE_MAKEFILE
    OFF
    CACHE BOOL "OFF" FORCE
)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if(${FORCE_COLORED_OUTPUT})
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  endif()
endif()
