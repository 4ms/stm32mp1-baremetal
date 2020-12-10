#!/usr/bin/env bash

arm-none-eabi-as -o startup.o startup.s
arm-none-eabi-ld -T linkscript.ld -o better-hang.elf startup.o
arm-none-eabi-objcopy -O binary better-hang.elf better-hang.bin
../u-boot-stm32mp-2020.01-r0/u-boot-stm32mp-2020.01/build/tools/mkimage -A arm -C none -T kernel -a 0xC2000040 -e 0xC2000040 -d better-hang.elf bare-arm.uimg
