[ "$#" -eq 1 ] || { 
	echo "Usage: ./format-sdcard.sh /dev/sdX" >&2; 
	echo "Where /dev/sdX is the sd card device, e.g. /dev/sdc"
	exit 1; 
}

if [ ! -b $1 ]; then
	echo "Device $1 does not exist";
	exit 1;
fi

echo "Device $2 found."
echo "Formatting..."

mkfs.ext2 $1

echo "Done"
