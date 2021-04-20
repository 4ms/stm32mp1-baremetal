#!/usr/bin/env bash

[ "$#" -eq 2 ] || { 
	echo "Usage:"
	echo "scripts/copy_to_sdcard.sh path/to/application.uimg /dev/XXX" >&2;
	echo ""
	echo "Where /dev/XXX is the name of the 4th partition, e.g. /dev/disk1s4 or /dev/sdc4"
	echo ""
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

case "$(uname -s)" in
	Darwin)
		UNMOUNTCMD="sudo diskutil unmount $2"
		MOUNTCMD="sudo diskutil mount -mountPoint /tmp/sdcard_root $2"
		;;
	Linux)
		UNMOUNTCMD="sudo unmount $2"
		MOUNTCMD="sudo mount -o user -t FAT32 $2 /tmp/sdcard_root"
		;;
	*)
		echo 'OS not supported'
		;;
esac

echo ""
echo "Device $2 found, unmounting..."
echo $UNMOUNTCMD 
eval $UNMOUNTCMD

echo ""
echo "Mounting on /tmp/sdcard_root"
rm -rf /tmp/sdcard_root
mkdir -p /tmp/sdcard_root
eval $MOUNTCMD || (echo "ERROR: can't mount" && exit 0)

echo ""
echo "Current contents of sdcard:"
echo "ls -l /tmp/sdcard_root/"
ls -l /tmp/sdcard_root/

if [ -e /tmp/sdcard_root/bare-arm.uimg ]; then
	echo "";
	echo "Deleting bare-arm.uimg";
	echo "sudo rm /tmp/sdcard_root/bare-arm.uimg";
	sudo rm /tmp/sdcard_root/bare-arm.uimg;
fi

echo ""
echo "Copying $1 to bare-arm.uimg on sdcard"
echo "sudo cp $1 /tmp/sdcard_root/bare-arm.uimg"
sudo cp $1 /tmp/sdcard_root/bare-arm.uimg

echo ""
echo "New contents of sdcard:"
ls -l /tmp/sdcard_root/

echo ""
echo "Unmounting sdcard..."
eval $UNMOUNTCMD

echo "Done!"
