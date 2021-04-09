#!/usr/bin/env bash

arm-none-eabi-as -o startup.o startup.s
arm-none-eabi-gcc -c -nostdlib -nostartfiles -o cstart.o cstart.c
arm-none-eabi-ld -T linkscript.ld -o cenv.elf startup.o cstart.o
arm-none-eabi-objcopy -O binary cenv.elf cenv.bin

# make -C ../common_uboot/ vexpress_ca9x4_config ARCH=arm CROSS_COMPILE=arm-none-eabi-
# make -C ../common_uboot/ all ARCH=arm CROSS_COMPILE=arm-none-eabi-

../u-boot-stm32mp-2020.01-r0/u-boot-stm32mp1-baremetal/tools/mkimage -A arm -C none -T kernel -a 0xC0000040 -e 0xC0000040 -d cenv.bin bare-arm.uimg
#./create-sd.sh sdcard.img bare-arm.uimg
