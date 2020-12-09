Compilation of U-Boot:
1. Pre-requisite
2. Initialise cross-compilation via SDK
3. Prepare U-Boot source code
4. Management of U-Boot source code
5. Compile U-Boot source code
6. Update software on board

1. Pre-requisite:
-----------------
OpenSTLinux SDK must be installed.

For U-Boot build you need to install:
* libncurses and libncursesw dev package
    - Ubuntu: sudo apt-get install libncurses5-dev libncursesw5-dev
    - Fedora: sudo yum install ncurses-devel
* git:
    - Ubuntu: sudo apt-get install git-core gitk
    - Fedora: sudo yum install git

If you have never configured you git configuration:
    $ git config --global user.name "your_name"
    $ git config --global user.email "your_email@example.com"

2. Initialize cross-compilation via SDK:
---------------------------------------
* Source SDK environment:
    $ source <path to SDK>/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi

* To verify if you cross-compilation environment are put in place:
    $ set | grep CROSS
    CROSS_COMPILE=arm-ostl-linux-gnueabi-

Warning: the environment are valid only on the shell session where you have
         sourced the sdk environment.

3. Prepare U-Boot source:
------------------------

Extract the sources from tarball, for example:
$ tar xfJ SOURCES-st-image-weston-openstlinux-weston-stm32mp1-*.tar.xz

In the U-Boot source directory (sources/*/u-boot-stm32mp-2020.01-r0),
you have one U-Boot source tarball, the patches and one Makefile:
   - u-boot-stm32mp-2020.01-r0.tar.gz
   - 000*.patch
   - Makefile.sdk

NB: if you would like to have a git management on the code see
    section 4 [Management of U-Boot source code with GIT]

Then you must extract the tarball and apply the patch:

    $> tar xfz u-boot-stm32mp-2020.01-r0.tar.gz
    $> cd u-boot-stm32mp-2020.01
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

4. Management of U-Boot source code with GIT
--------------------------------------------
If you like to have a better management of change made on U-Boot source,
you have 3 solutions to use git

4.1 Get STMicroelectronics U-Boot source from GitHub

    URL: https://github.com/STMicroelectronics/u-boot.git
    Branch: v2020.01-stm32mp
    Revision: v2020.01-stm32mp-r0

    $ git clone https://github.com/STMicroelectronics/u-boot.git
    $ git checkout -b WORKING v2020.01-stm32mp-r0

4.2 Create Git from tarball

    $ tar xfz u-boot-stm32mp-2020.01-r0.tar.gz
    $ cd u-boot-stm32mp-2020.01
    $ test -d .git || git init . && git add . && git commit -m "U-Boot source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 ../*.patch`; do git am $p; done

4.3 Get Git from community and apply STMicroelectronics patches

    URL: git://git.denx.de/u-boot.git
    Branch: master
    Revision: v2020.01

    $ git clone git://git.denx.de/u-boot.git
or
    $ git clone http://git.denx.de/u-boot.git

    $ cd u-boot
    $ git checkout -b WORKING v2020.01
    $ for p in `ls -1 ../*.patch`; do git am $p; done

5. Compilation U-Boot source code:
----------------------------------
To compile U-Boot source code, first move to U-Boot source:
    $ cd u-boot-stm32mp-2020.01
    or
    $ cd u-boot

5.1 Compilation for one target (one defconfig, one device tree)

    see <U-Boot source>/board/st/stm32mp1/README for details

    $ make stm32mp15_<config>_defconfig
    $ make DEVICE_TREE=<device tree> all

    example:

    a) trusted boot on ev1
	$ make stm32mp15_trusted_defconfig
	$ make DEVICE_TREE=stm32mp157c-ev1 all

    b) basic boot on dk2
	$ make stm32mp15_basic_defconfig
	$ make DEVICE_TREE=stm32mp157c-dk2 all

5.2 Compilation for several targets: use Makefile.sdk

Calls the specific 'Makefile.sdk' provided to compile U-Boot:
  - Display 'Makefile.sdk' file default configuration and targets:
    $> make -f $PWD/../Makefile.sdk help
  - Compile default U-Boot configuration:
    $> make -f $PWD/../Makefile.sdk all

Default U-Boot configuration is done in 'Makefile.sdk' file through two specific
variables 'DEVICE_TREE' and 'UBOOT_CONFIGS':
  - 'DEVICE_TREE' is a list of device tree to build, using 'space' as separator.
    ex: DEVICE_TREE="<devicetree1> <devicetree2>"
  - 'UBOOT_CONFIGS' is a list of '<defconfig>,<type>,<binary>' configurations,
        <defconfig> is the u-boot defconfig to use to build
        <type> is the name append to u-boot binaries (ex: 'trusted', 'basic', etc)
        <binary> is the u-boot binary to export (ex: 'u-boot.bin', 'u-boot.stm32', etc)
    ex: UBOOT_CONFIGS="<defconfig1>,basic,u-boot.bin <defconfig1>,trusted,u-boot.stm32"

The generated binary files are available in ../build-${config}.

by default we define 3 configs: basic, trusted, optee for the several boards
The generated files are :
  for trusted and optee configs:
    #> ../build-{trusted,optee}/*.stm32
  for basic config
    #> ../build-basic/u-boot-spl.elf-*-basic
    #> ../build-basic/u-boot-*-basic.img

You can override the default U-Boot configuration if you specify these variables:
  - Compile default U-Boot configuration but applying specific devicetree(s):
    $ make -f $PWD/../Makefile.sdk all DEVICE_TREE="<devicetree1> <devicetree2>"
  - Compile for a specific U-Boot configuration:
    $ make -f $PWD/../Makefile.sdk all UBOOT_CONFIGS=<u-boot defconfig>,<u-boot type>,<u-boot binary>
  - Compile for a specific U-Boot configuration and applying specific devicetree(s):
    $ make -f $PWD/../Makefile.sdk all UBOOT_CONFIGS=<u-boot defconfig>,<u-boot type>,<u-boot binary> DEVICE_TREE="<devicetree1> <devicetree2>"

6. Update software on board:
----------------------------

see also <U-Boot source>/board/st/stm32mp1/README

6.1. partitioning of binaries:
------------------------------

There are two possible boot chains available:
- Basic boot chain (for basic configuration)
- Trusted boot chain (for trusted and optee configuration)

U-Boot build provides binaries for each configuration:
- Basic boot chain: U-Boot SPL and U-Boot imgage (for FSBL and SSBL)
- Trusted boot chain: U-Boot binary with ".stm32" extension (for SSBL, FSBL is provided by TF-A)

6.1.1. Basic boot chain:
On this configuration, we use U-Boot SPL as First Stage Boot Loader (FSBL) and
U-Boot as Second Stage Boot Loader (SSBL).
U-Boot SPL (u-boot-spl.stm32-*) MUST be copied on a dedicated partition named "fsbl1"
U-Boot image (u-boot*.img) MUST be copied on a dedicated partition named "ssbl"

6.1.2. Trusted boot chain:
On this configuration, U-Boot is associated to Trusted Firmware (TF-A) and only
U-Boot image is used as Second Stage Boot Loader (SSBL).
TF-A binary (tf-a-*.stm32) MUST be copied on a dedicated partition named "fsbl1"
U-boot binary (u-boot*.stm32) MUST be copied on a dedicated partition named "ssbl"

6.2. Update via SDCARD:
-----------------------

6.2.1. Basic boot chain
* u-boot-spl.stm32-*
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "fsbl1" is the partition 1:
  - SDCARD: /dev/mmcblkXp1 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX1 (where X is the instance number)
  $ dd if=<U-Boot SPL file> of=/dev/<device partition> bs=1M conv=fdatasync

* u-boot*.img
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "ssbl" is the partition 4:
  - SDCARD: /dev/mmcblkXp3 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX3 (where X is the instance number)
  $ dd if=<U-Boot image file> of=/dev/<device partition> bs=1M conv=fdatasync

6.2.2. Trusted boot chain
* tf-a-*.stm32
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "fsbl1" is the partition 1:
  - SDCARD: /dev/mmcblkXp1 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX1 (where X is the instance number)
  $ dd if=<TF-A binary file> of=/dev/<device partition> bs=1M conv=fdatasync

* u-boot*.stm32
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "ssbl" is the partition 4:
  - SDCARD: /dev/mmcblkXp3 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX3 (where X is the instance number)
  $ dd if=<U-Boot stm32 binary file> of=/dev/<device partition> bs=1M conv=fdatasync

6.2.3. FAQ
to found the partition associated to a specific label, just plug the
SDCARD/USB disk on your PC and call the following command:
  $ ls -l /dev/disk/by-partlabel/
total 0
lrwxrwxrwx 1 root root 10 Jan 17 17:38 bootfs -> ../../mmcblk0p4
lrwxrwxrwx 1 root root 10 Jan 17 17:38 fsbl1 -> ../../mmcblk0p1     ➔ FSBL (TF-A)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 fsbl2 -> ../../mmcblk0p2     ➔ FSBL backup (TF-A backup – same content as FSBL)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 rootfs -> ../../mmcblk0p5
lrwxrwxrwx 1 root root 10 Jan 17 17:38 ssbl -> ../../mmcblk0p3      ➔ SSBL (U-Boot)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 userfs -> ../../mmcblk0p6

6.3. Update via USB mass storage on U-Boot:
-------------------------------------------

We are using the U-Boot command ums

STM32MP> help ums
 ums - Use the UMS [USB Mass Storage]
 
 Usage:
 ums <USB_controller> [<devtype>] <dev[:part]>  e.g. ums 0 mmc 0
     devtype defaults to mmc
 ums <USB controller> <dev type: mmc|usb> <dev[:part]>

By default on STMicroelectronics board, "mmc 0" is SD card on SDMMC1.

* Plug the SDCARD on Board.
* Start the board and stop on U-Boot shell:
 Hit any key to stop autoboot: 0
 STM32MP>
* plug an USB cable between the PC and the board via USB OTG port.
* On U-Boot shell, call the usb mass storage functionality:
 STM32MP> ums 0 mmc 0
* After a delay (of up to 15 seconds), the host sees the exported block device.
* Follow section 6.2 to put U-Boot SPL binary and U-Boot binary
  (*.img or *.stm32) on SDCARD/USB disk.

PS: A Ctrl-C is needed to stop the command.