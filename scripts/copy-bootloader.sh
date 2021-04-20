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

[ "$#" -eq 2 ] || { 
	echo "Usage: scripts/copy-bootloader.sh /dev/XXX build/" >&2; 
	echo "Where /dev/XXX is the sd card device, e.g. /dev/sdc or /dev/disk3"
	echo "And 'build/' is the path to the u-boot image files, (make sure it ends in a '/')"
	exit 1; 
}

if [ ! -b $1 ]; then
	echo "Device $1 does not exist";
	exit 1;
fi

echo "Copying bootloaders... (this only works on macOS, for linux just remove the 's' in the of= part of the commands"

sudo dd if=$2/u-boot-spl.stm32 of=$1s1
sudo dd if=$2/u-boot-spl.stm32 of=$1s2 
sudo dd if=$2/u-boot.img of=$1s3

echo "Done!"

