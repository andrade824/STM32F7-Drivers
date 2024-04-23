# STM32F7 HAL and Simple RTOS
This repository contains a set of drivers for the STM32F7 platform that can be used for building other projects.

The code is designed for the <a href="http://www.st.com/en/evaluation-tools/32f746gdiscovery.html">32F746GDISCOVERY board</a> as well as my own custom development board, but should work with most STM32F7 devices with minimal changes.

## Drivers/Features
The current driverset includes drivers for the following hardware modules:
* GPIO
* UART
* SPI
* Timers
* DMA2D Controller
* FMC SDRAM Controller
* LCD Controller
* SDMMC Controller
* Nokia 5110 Display Controller
* RFM69 Radio Module

The following simple RTOS features are also supported:
* Task management
* Memory management
* FAT32 Filesystem
* Simple Font/Graphics rendering

## Code Organization
The following describes the folder structure and some useful files:
* **Makefile**: Used for compiling and debugging. Read through this first to get familiar with how I've set up the project.
* **.gdbinit_[jlink|openocd]**: Initialization script for GDB. This will automatically connect to an OpenOCD GDB server, enable ARM semihosting, reset the CPU, load the binary, set a breakpoint on the "run()" function, run to that breakpoint, and start up GDB Text UI mode.
* **apps/**: This folder is meant to hold any future applications that utilize the drivers in this project. This folder also contains any applications that have the sole purpose of testing the drivers.
* **drivers/**: This folder contains the code for any of the peripherals in the STM32F7 microcontroller.
    * **registers/**: Header files that contain macros and structures for easily accessing device registers. Refer to platform/bitfield.h for understanding how these files are developed.
* **output/**: Contains the object and executable files.
* **platform/**: Contains any ARM-generic and startup code. This code utilizes the CMSIS library for easy access to ARM peripherals. Wrappers around some of the CMSIS function are being developed that maintain my coding style and provide better input-checking (see interrupt.c for an example).
    * **bitfield.h**: Contains macros that are used to generate inline functions that set/get bitfields within registers. The "drivers/registers/*" files use these macros extensively. This provides for easier to read code than manually setting registers to various hex values.

## Toolchain and Compiling
All of my development work has taken place on Mac/Linux so the following tools might not be readily available on other operating systems.

What you'll need:
* <a href="https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads">GNU ARM Embedded Toolchain</a> for compiler, linker, newlib standard library, and binutils.
* Latest version of <a href="https://github.com/ntfreak/openocd">OpenOCD</a> (0.12.0 as of writing) for programming and debugging. This software provides a GDB server that interacts with the ST-Link debugger built-in to the board. An alternative to OpenOCD is <a href="https://github.com/texane/stlink">texane's st-link utility</a>, but it currently doesn't support ARM semihosting (a debug function that I utilize to print debug messages to the host's console). For my custom development board I use J-Link debuggers along with the J-Link GDB server.
* Patience.

After you install the above software, you need to make sure the binaries are added to your PATH (so the makefile can find them).

On Linux, before you can start debugging, OpenOCD requires a little bit more setup work. By default, OpenOCD requires "sudo" to access USB devices. You'll probably want to add yourself to the 'plugdev' group and then add a udev rule for the ST-Link programmer. In the OpenOCD source code, <a href="https://github.com/ntfreak/openocd/blob/master/contrib/60-openocd.rules">'contrib/60-openocd.rules'</a> contains the correct rule for the ST-Link device that ensures the USB device file is a part of the 'plugdev' group.

### Makefile Targets
Once the tools are setup, using the makefile is simple. Here are the available targets:
* **all**: Compile both release and debug binaries.
* **release**: Compile a binary with full optimization enabled as well as no semihosting support (all "dbprintf()" calls are compiled out).
* **debug**: Compile a binary with only optimizations that don't affect debugging and full semihosting support. The ARM semihosting support allows the device to print to the debugger terminal. Since ARM semihosting requires a debugger connected to work, the application will hang at the first semihosting call it executes if no debugger is connected. If you need the application to run without a debugger, use the "release" target.
* **gdb_[openocd|jlink]**: Kick open GDB to debug an executable with debug symbols (check the supplied ".gdbinit_[openocd|jlink]" file to get familiar with the default settings). Will compile the "debug" target if not already done.
* **[openocd|jlink]**: Starts an OpenOCD/J-Link GDB server that GDB will connect to. This must be started before opening GDB with the above target.
* **burn_[openocd|jlink]**: Burn the release version of the binary to the board. Will compile the "release" target if not already done.
* **size**: Print out the size of any compiled executables.
