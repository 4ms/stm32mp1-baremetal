#!/usr/bin/env bash

[ "$#" -eq 2 ] || { 
	echo "Usage: ./copy_to_sdcard.sh {path/to/application.uimg} /dev/{sdcard device name}" >&2;
	exit 1;
}

if [ ! -f $1 ]; then
	echo "Application uimg file $1 does not exist";
	exit 1;
fi

if [ ! -b $2 ]; then 
	echo "Error: Device $2 does not exist."; 
	exit 1;
fi

echo "Device $2 found, mounting..."
mkdir -p /tmp/sdcard_root
sudo mount -o user $2 /tmp/sdcard_root/
echo "Current contents of sdcard:"
ls -l /tmp/sdcard_root/
echo "Deleting bare-arm.uimg"
sudo rm /tmp/sdcard_root/bare-arm.uimg
echo "Copying $1 to bare-arm.uimg on sdcard"
sudo cp $1 /tmp/sdcard_root/bare-arm.uimg
echo "New contents of sdcard:"
ls -l /tmp/sdcard_root/
echo "Unmounting sdcard..."
sudo umount $2
echo "Done!"
