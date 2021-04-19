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

[ "$#" -eq 2 ] || { 
	echo "Usage: scripts/partition-sdcard.sh /dev/XXX build/" >&2; 
	echo "Where /dev/XXX is the sd card device, e.g. /dev/sdc or /dev/disk3"
	echo "And 'build/' is the path to the u-boot image files, (make sure it ends in a '/')"
	exit 1; 
}

if [ ! -b $1 ]; then
	echo "Device $1 does not exist";
	exit 1;
fi

echo "Device $1 found, removing partitions"
sudo sgdisk -o $1 

echo "Partitioning..."
sudo sgdisk --resize-table=128 -a 1 -n 1:34:545 -c 1:fsbl1 -n 2:546:1057 -c 2:fsbl2 -n 3:1058:5153 -c 3:ssbl -N 4 -c 4:prog -p $1

echo "Copying bootloaders... (this only works on macOS, for linux just remove the 's' in the of= part of the commands"

sudo dd if=$2/u-boot-spl.stm32 of=$1s1
sudo dd if=$2/u-boot-spl.stm32 of=$1s2 
sudo dd if=$2/u-boot.img of=$1s3

echo "Done!"

