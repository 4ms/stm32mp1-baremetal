#!/usr/bin/env bash

set -e

[ "$#" -eq 1 ] || { 
	echo ""
	echo "Usage: "
	echo "scripts/format-sdcard.sh /dev/sdX" >&2;
	echo ""
	echo "Where /dev/sdX is the sd card device, e.g. /dev/sdc"
	exit 1; 
}

if [ ! -b $1 ]; then
	echo ""
	echo "Error: Device $1 does not exist";
	exit 1;
fi

echo ""
echo "Device $1 found."
echo "Formatting..."

set -x

KERNEL_NAME=$(uname -s)
case "${KERNEL_NAME}" in
	Darwin)
		diskutil eraseDisk FAT32 BAREMETA $1
		;;
	Linux)
		wipefs --all --backup $1
		mkfs.fat -F 32 $1
		;;
	*)
		echo 'OS not supported: please format $1 partition 4 as FAT32'
		;;
esac

echo "Done"
