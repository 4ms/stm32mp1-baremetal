arm-none-eabi-as -mcpu=arm926ej-s -g startup.s -o startup.o 
#arm-none-eabi-gcc -c -mcpu=arm926ej-s -g main.c -o main.o
arm-none-eabi-g++ -std=c++20 -fno-threadsafe-statics -fno-exceptions -ffreestanding -c -mcpu=arm926ej-s -g main.cpp -o main.o
arm-none-eabi-g++ -specs=nosys.specs -specs=nano.specs -T linkscript.ld  main.o startup.o -o main.elf 
arm-none-eabi-objcopy -O binary main.elf main.bin
qemu-system-arm -M versatilepb -m 128M -nographic -kernel main.bin 
