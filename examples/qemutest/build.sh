arm-none-eabi-as -o startup.o startup.s
arm-none-eabi-g++ -O0 -mthumb -fno-builtin -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -std=c++20 -fno-exceptions -o main.o -c main.cpp
arm-none-eabi-g++ -T linkscript.ld -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs -mthumb -mabi=aapcs -o qemutest.elf startup.o main.o
arm-none-eabi-objcopy -O binary qemutest.elf qemutest.bin
