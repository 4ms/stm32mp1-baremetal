## Audio Processor project

This project runs on the STM32MP157 Discovery board, using the board's codec and mic/line jack.

[to be continued...]

To use, first make sure U-Boot is installed on an SD card, as usual (see README
in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the a7-main.uimg file. Copy it to the 4th
partition of the SD-card, as usual (again, see README in the project root
dir).
