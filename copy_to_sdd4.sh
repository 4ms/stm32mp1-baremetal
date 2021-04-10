#!/usr/bin/env bash

[ "$#" -eq 1 ] || { echo "Usage: ./copy_to_sdd4.sh path/to/bare-arm.uimg" >&2; exit 1; }

if [ ! -f $1 ]; then
	echo "File $1 does not exist";
	exit 1;
fi

if [ -b "/dev/sdd4" ] 
then
    echo "Device /dev/sdd4 exists." 
	echo "Mounting..."
	sudo mount -o user /dev/sdd4 ~/sdcard_root/
	echo "Current contents:"
	ls -l ~/sdcard_root/
	echo "Deleting bare-arm.uimg"
	sudo rm ~/sdcard_root/bare-arm.uimg
	echo "Copying $1 to /bare-arm.uimg"
	sudo cp $1 ~/sdcard_root/bare-arm.uimg
	echo "New contents:"
	ls -l ~/sdcard_root/
	echo "Unmounting"
	sudo umount /dev/sdd4
else
    echo "Error: Device /dev/sdd4 does not exist."
	echo "Can't copy $1 to the device."
fi
