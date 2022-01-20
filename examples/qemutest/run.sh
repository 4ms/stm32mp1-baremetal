qemu-system-arm -S -M vexpress-a9 -m 512M -no-reboot -nographic -gdb tcp::2159 -monitor telnet:127.0.0.1:1234,server,nowait -kernel build/c4core-quickstart.bin
