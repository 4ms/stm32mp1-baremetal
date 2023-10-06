## Setting up your environment

### Requirements:

- gcc arm embedded toolchain, version 10 or later (version 12.3 recommended).
  - Download here: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
    - Make sure you get the package for your host computer that ends in `arm-none-eabi`
  - Or xpack versions are OK: https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/tag/v12.2.1-1.2
  - Make sure `arm-none-eabi-gcc` is on your PATH (this is the "bin" folder in
    the `arm-non-eabi` distribution).
  - **IMPORTANT NOTE FOR WINDOWS USERS**: The PATH that `arm-none-eabi` is
    installed to *must not* contain any spaces. By default, running the
    `arm-none-eabi` installer (the download that ends in ".exe"), installs to
    "C:\Program Files (x86)\\..." which contains spaces. It is much easier to
    download the file ending in ".zip", which allows you to place the
    binaries wherever you want. As an example, you could unzip the files to
    "C:\arm-none-eabi" or similar.
- Python 3.x to run the uimg header script
- A command-line shell (MacOS and Linux have this built-in, but for Windows you will need MSYS/MinGW or something else).


### MacOS

A recent-ish macOS version is required, though it's unknown exactly how old will still work.
Monterey (12.5) and Ventura (13.4 and 13.5) have been tested and are known to work.

 - Install [brew](https://brew.sh) and follow the "Next Steps" instructions to add Homebrew to your PATH.

 - Install the arm-non-eabi toolchain using brew: `brew install --cask gcc-arm-embedded`
    - Alternatively, you can download a tar.gz or .pkg installer from [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
      - Make sure you get the package for your host computer that ends in `arm-none-eabi` ("AArch32 bare-metal target").
      - If you download the .tar.gz file, unzip it and add the `bin/` dir to
        your PATH. Tip: if you don't know what that means, use a different
        method (.pkg installer or the brew command above).
      - Sometimes the OS security system flags the executables the first
        time they are run. Just keep running "make clean" and then "make" and then
        clicking "Allow" until you get no more security pop-ups (might be 6-10
        pop-ups). It only happens the first time after installation.
    - Another alternative is to use the [xpack installer](https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/tag/v12.2.1-1.2)

### Linux
 
 - [Tips for installing arm-none-eabi on Ubuntu](https://lindevs.com/install-arm-gnu-toolchain-on-ubuntu)
 - python3, git, and GNU make should already be installed for your system. If not, something like `sudo apt-get update && sudo apt-get install git make python3` should work.
 
### Windows

- Install [MSYS](http://www.msys2.org/) and then open the MinGW 64-bit shell and run:

```
pacman -Syu git make python 
```

  - Be sure to always be using the MinGW 64-Bit shell (Blue icon)! Other shells won't work

 - Install the `arm-none-eabi` package:
    - Download the .exe installer from [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
      - Make sure you get the .exe installer for Windows that ends in `arm-none-eabi` ("AArch32 bare-metal target").
      - Run the installer from a MinGW 64 shell.
      - You may need to add this to your MinGW PATH. Close all terminal
        prompts, and open C:\msys64\home\\(USERNAME)\\.bashrc in a text editor
        of your choice. Add the following line (you might need to change this
        depending on if `arm-none-eabi` has a different version number since
        this document was written):
        `export PATH=$PATH:"/c/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/12.3 rel1/bin"`

    - Alternatively, download the .zip file and unzip it into a directory in your MinGW paths (`/home/USERNAME/....')
      - Add the location of the unzipped folder (plus `bin/` at the end), to your MinGW PATH:
        - Close all terminal prompts, and open
          C:\msys64\home\\(USERNAME)\\.bashrc in a text editor of your choice.
          Add the following line (you might need to change this depending on if
          `arm-none-eabi` has a different version number since this document
          was written): 
          `export PATH=$PATH:/home/USERNAME/arm-gnu-toolchain-12.3.rel1-mingw-w64-i686-arm-none-eabi/bin`

 - To test this, open a new MinGW 64-bit shell and type:
    ```
    arm-none-eabi-gcc --version
    ```

    You should see the version number and some copyright info. If not, check where
    the arm-none-eabi package was installed and adjust your PATH setting.

