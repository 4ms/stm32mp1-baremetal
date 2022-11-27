# USB MIDI Host


This project demonstrates the use of the STM32 USB Host Library by creating a
MIDI Host. Plug in a MIDI device such as a musical keyboard or sequencer or controller.
Playing notes or changing CCs will display on the console.

The board can provide VBus power (+5V) to the external device.

Modify `main.cc` to either select the OSD32MP1-BRK board or a Discovery board.

Since the OSD32MP1-BRK board is
typically powered through the USB jack, you need to power the 

When using the OSD32MP1-BRK board, you will need to power the board from the
VIN and GND pins using a 5V supply, instead of powering it from the USB jack.
This frees up the USB jack to use connect to a MIDI device. You probably will 
need to use a "USB OTG" (On The Go) adaptor that goes from micro-USB to female
type-A, and then another cable that goes from male type-A to whatever your MIDI
device has.

When using the STM32MP157C-DK2 Discovery board, power the board normally using
the USB-C jack next to the SD-Card slot. The other USB-C jack can supply VBus
to the MIDI device, but there is USB-C Interface IC (STUSB1600) responsible
for enabling VBus. The example code configures this IC properly via I2C.

In this directory run:

```
make
```

To load the program onto your SD Card, follow the normal procedure (see
[here](https://github.com/4ms/stm32mp1-baremetal/blob/master/README.md#6-copy-the-application-to-the-sd-card) )

Power up and you should see this in the console:

```
USB MIDI Host test
Connect a USB cable to a MIDI device (keyboard, etc)
 
[waits here until you plug in a MIDI device]

USB Device Connected
USB Device Reset Completed
PID: 288h
VID: 1c75h
Address (#1) assigned.
Manufacturer : Arturia
Product : Arturia KeyStep 32
Serial Number : 00000000001A
Enumeration done.
This device has only 1 configuration.
Default configuration set.
DEBUG : Found Audio Control subclass

Switching to Interface (#1)
Class    : 1h
SubClass : 3h
Protocol : 0h
MIDI class started.
Class selected
Class active

[when you play on the MIDI device you'll see data like this:]
Note: 62 Vel: 108
Note: 62 off
Note: 57 Vel: 83
Note: 57 off
Note: 60 Vel: 79
Note: 60 off
```

The green LED will flash regularly to indicate the application is running, and the red 
LED will toggle whenever a MIDI message is received.


## Dependencies and modifications

  * STM32 USB Host Library v3.5.0 is used here, with one modication:

	- `Core/Src/usbh_ctlreq.c`: `USBH_ParseCfgDesc()` contains a bug that hangs
	  when a MIDI Streaming device is connected. I patched this, and it's been reported
	  to STM [here](https://github.com/STMicroelectronics/stm32_mw_usb_host/issues/11#issuecomment-1312278910)
  
  * STM32CubeH7 HAL drivers for usb are used here, with some modifications:

      * All files: "h7" changed to "mp1" (found in comments, #includes, include guards)

	  * `stm32mp1xx_hal_hcd.c/.h`: no modifications

	  * `stm32mp1xx_ll_usb.c/.h`: selectively took some modifications from the hftrx project:
		- Added constant `USB_OTG_HS_EMBEDDED_PHY`
		- Added a section in `USB_CoreInit()` to initialize global USB registers.
		- Commented out the name of a register that doesn't appear in the ST32MP157
		  CMSIS-device file (`GDFIFOCFG`)
		- In `USB_DevInit()`, initialize the Device Speed in the same way as with
		  an `OTG_ULPI_PHY`
		- Altered logic in `USB_EPStartXfer()` for when to set the even/odd frame.
		- In `USB_HostInit()`, `USB_HC_Init()` and `USB_HC_StartXfer()`, don't use
		  the CID register to determine if we're HS capable (always assume it's
		  true)

	  * `stm32mp1xx_ll_usb_phy.c/.h`: New files. These contain initialization code for the USBPHY peripheral. 
		It's largely based on the implementation of such from the hftrx project.


  * `usbh_conf.c/.h`: Copied from [STM32CubeH7 example application USB Host
	Audio](https://github.com/STMicroelectronics/STM32CubeH7/blob/master/Projects/STM32H743I-EVAL/Applications/USB_Host/AUDIO_Standalone/Src/usbh_conf.c)
	with these modifications:

	- Re-wrote `HAL_HCD_MspInit/DeInit` to use the MP1 registers. Turns on OTG
	  and USBPHY clocks, and connects OTG to the port we use
	- Removed the use of a global `HCD_HandleTypeDef`. Instead we put it in
	  MidiHost, so there's no issue if we ever need multiple MIDI Hosts
	- `USBH_LL_Init()`: does not init the HCD_Handle
	- Elsewhere we init the number of host channels to 16, and selected the
	  right phy interface
	- Removed Board-specific stuff like VBUS enable driving
	- Fixed `USBH_LL_Set/GetToggle` not using the phost argument, but instead
	  directly accessing the global handle

  * `usbh_midi.cc/hh`:  I started with the [CDC Host Class from the STM USB
	Library](https://github.com/STMicroelectronics/stm32_mw_usb_host/blob/master/Class/CDC/Src/usbh_cdc.c)
	and refactored it to match the MIDI USB specifications:

	- Renamed anything CDC to MIDI
	- Removed the code about line encoding.
	- Changed the class and subclass to AudioClass and MidiStreamingSubclass
	- Made the control interface descriptor optional, and selected the data interface.
	- Refactored for C++ (scoped enums, constexpr, function object
	  callbacks)
	- Removed use of `USBH_malloc/free`, and instead let the application
	  own/manager the host class instance handle (i.e. `XXX_HandleTypeDef`) and
	  pass it to the host driver `USBH_RegisterClass`.
	- Refactored many common operations (like linking endpoints, opening
	  pipes), moving them to a `UsbHostHelper` class
	- Refactored the callbacks so we can pass function objects like a
	  lambda for the rx or tx callback.


### Limitations, Bugs ###


### Resources ###

ST has a good tutorial video playlist regarding USB and their library:
[MOOC STM32 USB training](https://www.youtube.com/playlist?list=PLnMKNibPkDnFFRBVD206EfnnHhQZI4Hxa)

The [hftrx project](https://github.com/ua1arn/hftrx) is the only other example
I found of USB working on baremetal STM32MP1. As noted above, many of the
modifications necessary were already done here, and I copied many portions of code from here.

