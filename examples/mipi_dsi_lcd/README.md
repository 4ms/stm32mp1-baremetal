# Credit to https://github.com/cody84/stm32mp1-baremetal/tree/master/examples/dk2_dsi-lvgl

## STM32MP157C-DK2 DSI with LVGL project

This project runs on the STM32MP157C-DK2 and demostrates double buffering and 3 methods of transfering a buffer of pixel data to a framebuffer. 

To use, first make sure u-boot is installed on an SD-card, as usual (see README in the project root directory). 

Clone the LVGL library to this directory. This project was built and tested on the release/v8.2 branch and is recommend for this project. 
```
cd [path to this directory]
git clone https://github.com/lvgl/lvgl.git --branch release/v8.2
```

Choose the display orientation, method to transfer pixel data to framebuffer, and a demo or one of the many examples found in lvgl/examples/  
To configure the display in portrait set:  
`#define H_RES 480`  
`#define V_RES 800`  
*** Note: In landscape you may notice a diagonal tearing line. This is because we exchange the row and column on the frame memory, but the display is still refreshed as 480x800. ***  
To configure the display in landscape set:  
`#define H_RES 800`  
`#define V_RES 480`  

To use 2 full screen size buffers and swap buffers when screen is updated set:  
`#define DOUBLE_BUFFER 1`  
To transfer a buffer of changed pixels using MDMA set:  
`#define MDMA_PIXEL_XFER 1`  
To transfer a buffer of changed pixels using DMA set:  
`#define DMA_PIXEL_XFER 1`  
To transfer a buffer of changed pixels using CPU (write pixel by pixel) set:  
`#define CPU_PIXEL_XFER 1`  
	
Select one of the demos listed in main.c and set necessary variables or chose one of the examples from lvgl/examples/ 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the a7-main.uimg file. Copy it to the 4th partition of the SD-card, as usual (again, see README in the project root dir).

Reboot your board and you should see the demo or example running on your STM32MP157C-DK2 Display. 

#### References/Credit

Parts of the startup.s code is based-on/inspired-by the `cenv` project for a Cortex-A9 simulator, from [umanovskis/baremetal-arm](https://github.com/umanovskis/baremetal-arm/tree/master/src/04_cenv), which is Copyright (c) 2018 Daniels Umanovskis under the MIT license. Other parts of the startup.s code are copied directly from the [ARM Cortex-A Series Programmer's Guide v4](https://developer.arm.com/documentation/den0013/d), and some parts I wrote myself. HAL DSI and LTDC libraries are ported from stm32f7xx_hal libraries. 

