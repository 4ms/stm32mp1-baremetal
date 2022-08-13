#!/usr/bin/env bash

# Script to install the first and second stage u-boot bootloaders
# into the first three partitions.
# The fourth partition is left empty.
#
# This script is meant to be run once when first installing the bootloader,
# or when the bootloader has changed and been re-compiled
# 
# After running this script, you must copy your application image file
# to the 4th partition

set -e

[ "$#" -eq 2 ] || [ "$#" -eq 1 ] || { 
	echo ""
	echo "Usage: scripts/copy-bootloader.sh /dev/XXX third-party/u-boot/build/" >&2; 
	echo ""
	echo "Where /dev/XXX is the sd card device, e.g. /dev/sdc or /dev/disk3"
	echo "And 'third-party/u-boot/build/' is the path to the dir containing the U-Boot image files, (make sure it ends in a '/')"
	echo ""
	echo "If you omit the last parameter, it defaults to third-party/u-boot/build/"
	exit 1; 
}

if [ ! -b $1 ]; then
	echo "Device $1 does not exist";
	exit 1;
fi

if [ "$#" -eq 1 ]; then
	path="third-party/u-boot/build"
else
	path=$2
fi

echo "Copying bootloader files..."

set -x

KERNEL_NAME=$(uname -s)
case "${KERNEL_NAME}" in
	Darwin)
		dd if=$path/u-boot-spl.stm32 of=${1}s1
		dd if=$path/u-boot-spl.stm32 of=${1}s2
		dd if=$path/u-boot.img of=${1}s3
		sleep 1
		diskutil unmountDisk $1
		;;
	Linux)
		# Clear existing data.
		dd if=/dev/zero of=${1}1 || true
	    dd if=/dev/zero of=${1}2 || true
		dd if=/dev/zero of=${1}3 || true
		# Write new bootloader.
		dd if=$path/u-boot-spl.stm32 of=${1}1
		dd if=$path/u-boot-spl.stm32 of=${1}2
		dd if=$path/u-boot.img of=${1}3
		sleep 1
		umount $1 || true
		;;
	*)
		echo 'OS not supported: please copy the u-boot images onto partitions 1, 2, and 3 like this:'
		echo "sudo dd if=$path/u-boot-spl.stm32 of=${1}s1"
		echo "sudo dd if=$path/u-boot-spl.stm32 of=${1}s2"
		echo "sudo dd if=$path/u-boot.img of=${1}s3"
		;;
esac


echo "Done!"

