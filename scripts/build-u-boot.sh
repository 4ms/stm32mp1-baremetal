#!/usr/bin/env bash

echo "Usage: scripts/build-u-boot.sh"

cd third-party/u-boot/u-boot-stm32mp1-baremetal
make O=../build CROSS_COMPILE=arm-none-eabi- stm32mp15x_baremetal_defconfig
make -j16 O=../build CROSS_COMPILE=arm-none-eabi- all
cd ../..
