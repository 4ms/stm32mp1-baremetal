#!/usr/bin/env bash

# Script to format and partition an SD card 
# and install the first and second stage u-boot bootloaders
# into the first three partitions.
# The fourth partition is left empty.
#
# This script is meant to be run once when using a new SD Card.
#
# 
# After running this script, you must load your application
# code on the card using the copy_to_sdcard.sh script

[ "$#" -eq 1 ] || { 
	echo "Usage: ./partition-sdcard.sh /dev/sdX" >&2; 
	echo "Where /dev/sdX is the sd card device, e.g. /dev/sdc"
	exit 1; 
}

if [ ! -b $1 ]; then
	echo "Device $1 does not exist";
	exit 1;
fi

echo "Device $2 found."
echo "Partitioning..."

sudo sgdisk -o $1 
sudo sgdisk --resize-table=128 -a 1 \ 
-n 1:34:545 -c 1:fsbl1 \
-n 2:546:1057 -c 2:fsbl2 \
-n 3:1058:5153 -c 3:ssbl \
-n 4:5154:9249 -c 4:prog -p $1

#fsbl1 = First Stage Bootloader 1
#fsbl1 = First Stage Bootloader 2 (duplicate/backup partition)
#ssbl = Second Stage Bootloader
#prog = can be any size and any name, it will be where the application code lives, eventually

echo "Copying bootloaders..."
sudo dd if=$KBUILD_OUTPUT/u-boot-spl.stm32 of=$1s1
sudo dd if=$KBUILD_OUTPUT/u-boot-spl.stm32 of=$1s2 
sudo dd if=$KBUILD_OUTPUT/u-boot.img of=$1s3

echo "Done!"

