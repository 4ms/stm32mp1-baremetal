## usb-basic project

This project demonstrates the use of ST's USB Device Library by creating an Mass Storage Class device.

Currently it's only been tested on the OSD32BRK board. [TODO: verify it works on the Discovery]

After you build and load this project as normal (see the top-level README.md), you should see this in the console when you boot:

```
Basic USB test
Connect a USB cable to a computer
You should see a 128MB unformatted drive appear.
```

When you connect the OSD32BRK board to a computer with a micro-USB cable, the computer should recognize that a new Mass Storage Class device is connected. You'll need to format it with whatever format you want. You can then use it like a normal USB thumb drive. But -- the memory is volatile so you'll lose everything if the OSD32 board loses power. 

If you really want to test persistance between USB connect/disconnect events, then you'll need to power the OSD32BRK board using the VIN pins (+5V), and also remove inductor L7 so that VIN doesn't flow into the computer. With this modification, you can keep the board powered while you plug and unplug.


### How this was made ###

The `ll_usb` and `hal_pcd` drivers have been "ported" (basically copied, with minor modifications) from STM32CubeH7.

Some of these modifications were from the [hftrx project here](https://github.com/ua1arn/hftrx)


### Limitations, Bugs ###

Currently, USB events are processed in the main loop. Using the OTG_IRQn interrupt was proving to be unstable -- the interrupt would just stop getting called after some time. So, to keep this example project simple and stable, I opted to handle events in the main loop. A real-life project will probably prefer interrupts.

### Changes to the ST USB Device Library: ###

[TODO]


### Resources ###

ST has a good tutorial video play list regarding USB and their library:
[MOOC STM32 USB training](https://www.youtube.com/playlist?list=PLnMKNibPkDnFFRBVD206EfnnHhQZI4Hxa)
