#!/usr/bin/env bash

if [ "$#" -ne 0 ]; then
	echo ""
	echo "Usage: "
	echo "scripts/build-u-boot.sh"
	echo ""
	exit 1
fi


echo "What board do you want to build for?"
echo "1) STM32MP157A-DK1"
echo "2) STM32MP157C-DK2"
echo "3) STM32MP157D-DK1"
echo "4) STM32MP157F-DK2"
echo "5) OSD32MP1-BRK"
read -p "Enter a number 1-5: " boardnum

DT=""

case $boardnum in
	"1")
		DT="stm32mp157a-dk1"
		;;
	"2")
		DT="stm32mp157c-dk2"
		;;
	"3")
		DT="stm32mp157d-dk1"
		;;
	"4")
		DT="stm32mp157f-dk2"
		;;
	"5")
		DT="stm32mp157c-osd32mp1-brk"
		;;
	*)
		echo "Please enter a number 1-5"
		exit 0
esac

cd third-party/u-boot

set -ex

make O=build mrproper
make O=build DEVICE_TREE=$DT CROSS_COMPILE=arm-none-eabi- stm32mp15_basic_defconfig
make -j16 O=build DEVICE_TREE=$DT CROSS_COMPILE=arm-none-eabi- all
