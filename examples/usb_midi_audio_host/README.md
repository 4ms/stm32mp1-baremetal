# USB MSC Device 

This project demonstrates the use of the STM32 USB Device Library by creating a
Mass Storage device Class (MSC). Plugging your board into your computer will make
it show up as a USB flash drive.

It's been tested on the OSD32MP1-BRK board, the STM32MP157C-DK2 Discovery
board, and the STM32MP157A-DK1 Discovery board.

To use, first make sure U-Boot is installed on an SD card, as usual (see
[README](https://github.com/4ms/stm32mp1-baremetal/blob/master/README.md) in
the project root directory). 

Optionally, modify `main.cc` to either select the OSD32MP1-BRK board or a
Discovery board. (All this does is select the GPIO pins for the green LED
-- it'll still run if you select the wrong board). 

Then, in this directory run:

```
make
```

In the `build/` dir, you should see the `a7-main.uimg` file. Copy it to the 4th
partition of the SD-card, as usual (again, see the
[README](https://github.com/4ms/stm32mp1-baremetal/blob/master/README.md) in
the project root dir). Power up and you should see this in the console:

```
Basic USB test
Connect a USB cable to a computer
You should see a 128MB unformatted drive appear.
```

When you connect the OSD32MP1-BRK or Discovery board to a computer with a
micro-USB cable, the computer should recognize that a new Mass Storage Class
device is connected. You'll need to format it and then you can then use it like
a normal USB thumb drive. (But the memory is volatile so you'll lose everything
if the board loses power!)

Tip: Since the OSD32MP1-BRK board is powered through the same USB jack that
supports OTG USB, it's difficult to test things like memory persistance across
USB connect/disconnect events.  But, it is possible to power the OSD32MP1-BRK
board from the VIN pins with a 5V supply. You also need to remove inductor L1
so that VIN doesn't flow into the computer. Once this is done, you can
plug/unplug the USB jack without disrupting power. The Discovery boards don't
need any modification, as they have separate USB jacks for power and OTG USB.


## How this was made

The `stm32mp1xx_ll_usb` and `stm32mp1xx_hal_pcd` drivers have been "ported"
(basically copied, with minor modifications) from STM32CubeH7.

Many of these modifications were from the [hftrx project
here](https://github.com/ua1arn/hftrx)

The `usbd_conf.c/h`, `usbd_desc.c/h`, and `usbd_msc_storage.c/h`files were
copied from the example `MSC_Standalone` demonstration project found in
STM32CubeH7 for the STM32H747I-DISCO board. They were modified a bit, as I saw
fit. Mainly, I changed `HAL_PCD_MspInit()` to enable the USBPHYC clocks, and
not do anything with GPIOs. In `usbd_msc_storage.c`, I added the I/O to the
"virtual disk" buffer, inspired by ST's tutorial video on MSC Devices. In
`USBD_LL_Init()`, some initialization values were changed (should be obvious).
When using the STM32 USB library, all those files just mentioned are project
files, so they're meant to be modified to suit your project's needs.


### Changes to the USB and PCD drivers: ###

Here's a summary of what changed from the official STM32CubeH7 HAL:

  * `stm32h7xx_ll_usb.c` ==> `stm32mp1xx_ll_usb.c`:
	- Added `USB_HS_PHYCInit()` and `USB_HS_PHYCDeInit()`, taken (and modified
	  slightly) from hftrx project.
	- Added a section in `USB_CoreInit()` to initialize global USB registers.
	  Taken and modified from hftrx project.
	- Commented out the name of a register that doesn't appear in the ST32MP157
	  CMSIS-device file (GDFIFOCFG)
	- In `USB_DevInit()`, initialize the Device Speed in the same way as with
	  an `OTG_ULPI_PHY`
	- Altered logic in `USB_EPStartXfer()` for when to set the even/odd frame.
	- In `USB_HostInit()`, `USB_HC_Init()` and `USB_HC_StartXfer()`, don't use
	  the CID register to determine if we're HS capable (always assume it's
	  true)
	- Add two parameters to `USB_HC_Init()` for Hub Address and Port Address
	  (not tested, but copied from hftrx project).

  * `stm32h7xx_ll_usb.h` ==> `stm32mp1xx_ll_usb.h`:
    - Added constant `USB_OTG_HS_EMBEDDED_PHY` (from hftrx project)

  * `stm32h7xx_hal_pcd.c` ==> `stm32mp1xx_hal_pcd.c`:
	- In `HAL_PCD_IRQHandler()`, set the SNAK bit in DIEPCTL (set a NAK for all
	  IN End Points) after a reset from the host
	- Use AXISS clock, not HCLK to determine turnaround time (since MP1's OTG
	  peripheral is clocked by AXISS)

Not changed:

  * `stm32h7xx_hal_pcd.h`, `stm32h7xx_hal_pcd_ex.c`, and
	`stm32h7xx_hal_pcd_ex.h` were copied directly with no modifications, just
	renamed them to `stm32mp1xx_hal_pcd*`

  * Nothing in the STM32 USB Device Library was changed.


### Limitations, Bugs ###

  * I have not tested DMA mode yet.

  * I've only tested Device mode, Host mode will be a separate example project

  * If you enter USB gadget mode during the U-boot process (by hitting a key to
	stop auto-boot and then typing `ums 0 mmc 0`), you have to power cycle the
	board before this example project will work again. TODO: Figure out why
	this is happening (The error happens in USB_CoreReset(), the CSRST flag
	never gets cleared by the USB core).

  * Note: earlier versions of this project would not work with IRQs. The issue
	was that the USB OTG IRQ Handler must be Level sensitive, not Edge
	sensitive. I believe the reason is that if the OTG peripheral asserts an
	IRQ in the time between when you have already entered the IRQ Handler but
	before you have read the IAR register, then reading IAR will set the IRQ
	state to Active and the second IRQ event will be missed because when we
	write EOIR later, the state will then go from Active to Idle. However, if
	the IRQ is Level sensitive, then reading the IAR register will set the
	state to Active+Pending, and then when we write EOIR the state will be set
	to Pending. Thus, the second IRQ will be caught. What I don't understand is
	why the interrupt registers (GINTSTS, GOTGINT, DIEPINTx, DOEPINTx, DAINT,
	etc.) do not reflect the new (second) interrupt's status until
	HAL_PCD_IRQHandler is entered a second time. I suspect there is some
	register that must be read twice to get the second IRQ's values, but I
	haven't found exactly what this is. If anyone can explain the precise
	reason Edge sensitive IRQs fail, I'd much appreciate it!


### Resources ###

ST has a good tutorial video playlist regarding USB and their library:
[MOOC STM32 USB training](https://www.youtube.com/playlist?list=PLnMKNibPkDnFFRBVD206EfnnHhQZI4Hxa)

The [hftrx project](https://github.com/ua1arn/hftrx) is the only other example
I found of USB working on baremetal STM32MP1. As noted above, many of the
modifications necessary were already done here, and I copied many portions of code from here.

