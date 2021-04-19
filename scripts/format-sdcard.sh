#!/usr/bin/env bash

[ "$#" -eq 1 ] || { 
	echo "Usage: scripts/format-sdcard.sh /dev/sdX" >&2; 
	echo "Where /dev/sdX is the sd card device, e.g. /dev/sdc"
	exit 1; 
}

if [ ! -b $1 ]; then
	echo "Device $1 does not exist";
	exit 1;
fi

echo "Device $1 found."
echo "Formatting..."

mkfs.fat32 $1 || diskutil eraseDisk FAT32 BAREMETA $1
echo "Done"
