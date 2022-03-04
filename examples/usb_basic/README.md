## usb-basic project

This project demonstrates the use of the STM32 USB Device Library by creating a Mass Storage Class device.

Currently it's only been tested on the OSD32BRK board. [TODO: verify it works on the Discovery]

To use, first make sure U-Boot is installed on an SD card, as usual (see README
in the project root directory). 

Then, in this directory run:

```
make
```

In the `build/` dir, you should see the `a7-main.uimg` file. Copy it to the 4th
partition of the SD-card, as usual (again, see README in the project root
dir). Power up and you should see this in the console:

```
Basic USB test
Connect a USB cable to a computer
You should see a 128MB unformatted drive appear.
```

When you connect the OSD32BRK board to a computer with a micro-USB cable, the
computer should recognize that a new Mass Storage Class device is connected.
You'll need to format it. You can then use it like a normal USB thumb drive.
(But the memory is volatile so you'll lose everything if the OSD32 board
loses power!)

If you want to test persistance between USB connect/disconnect events, then
you'll need to power the OSD32BRK board using the VIN pins (+5V), and also
remove inductor L1 so that VIN doesn't flow into the computer.


### How this was made ###

The `stm32mp1xx_ll_usb` and `stm32mp1xx_hal_pcd` drivers have been "ported" (basically copied, with minor modifications) from STM32CubeH7.

Many of these modifications were from the [hftrx project here](https://github.com/ua1arn/hftrx)

The `usbd_conf.c/h`, `usbd_desc.c/h`, and `usbd_msc_storage.c/h`files were
copied from the example `MSC_Standalone` demonstration project found in
STM32CubeH7 for the STM32H747I-DISCO board. There were modified a bit, as I saw
fit. Mainly, I changed `HAL_PCD_MspInit()` to enable the USBPHYC clocks, and
not do anything with GPIOs. In `usbd_msc_storage.c`, I added the I/O to the
"virtual disk" buffer, inspired by ST's tutorial video on MSC Devices. In
`USBD_LL_Init()`, some initialization values were changed (should be obvious).
When using the STM32 USB library, all those files just mentioned are project
files, so they're meant to be modified to suit your project's needs.


### Changes to the USB and PCD drivers: ###

Here's a summary of what changed from the official STM32CubeH7 HAL:

  - `stm32h7xx_ll_usb.c` ==> `stm32mp1xx_ll_usb.c`:
    - Added `USB_HS_PHYCInit()` and `USB_HS_PHYCDeInit()`, taken (and modified slightly) from hftrx project.
	- Added a section in `USB_CoreInit()` to initialize global USB registers. Taken and modified from hftrx project.
    - Commented out the name of a register that doesn't appear in the ST32MP157 CMSIS-device file (GDFIFOCFG)
	- In `USB_DevInit()`, initialize the Device Speed the same as is done with an `OTG_ULPI_PHY`
	- Altered logic in `USB_EPStartXfer()` for when to set the even/odd frame.
	- In `USB_HostInit()`, `USB_HC_Init()` and `USB_HC_StartXfer()`, don't use the CID register to determine if we're HS capable (always assume it's true)
	- Add two parameters to `USB_HC_Init()` for Hub Address and Port Address (not tested, but copied from hftrx project).

  - `stm32h7xx_ll_usb.h` ==> `stm32mp1xx_ll_usb.h`:
    - Added constant `USB_OTG_HS_EMBEDDED_PHY` (from hftrx project)

  - `stm32h7xx_hal_pcd.c` ==> `stm32mp1xx_hal_pcd.c`:
    - In `HAL_PCD_IRQHandler()`, set the SNAK bit in DIEPCTL (set a NAK for all IN End Points) after a reset from the host
	- Use AXISS clock, not HCLK to determine turnaround time (since MP1's OTG peripheral is clocked by AXISS)

Not changed:

  - `stm32h7xx_hal_pcd.h`, `stm32h7xx_hal_pcd_ex.c`, and
	`stm32h7xx_hal_pcd_ex.h` were copied directly with no modifications, just
	renamed them to `stm32mp1xx_hal_pcd*`

  - Nothing in the STM32 USB Device Library was changed.


### Limitations, Bugs ###

Currently, USB events are processed in the main loop. Using the `OTG_IRQn`
interrupt was proving to be unstable -- the interrupt would just stop getting
called after some time. So, to keep this example project simple, I opted to
handle events in the main loop. A non-trivial project will probably prefer
interrupts.

I have not tested DMA mode yet.

I've only tested this on the OSD32BRK board, not on the Discovery.

I've only tested Device mode, Host mode will be a separate example project (I think).


### Resources ###

ST has a good tutorial video play list regarding USB and their library:
[MOOC STM32 USB training](https://www.youtube.com/playlist?list=PLnMKNibPkDnFFRBVD206EfnnHhQZI4Hxa)

The [hftrx project](https://github.com/ua1arn/hftrx) is the only other example
I found of USB working on baremetal STM32MP1. As noted above, many of the
modifications necessary were already done here, and I copied many portions of code from here.

