#!/usr/bin/env bash

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
