# Connect to an already running OpenOCD 10.0-rc1 GDB Server.
# Use the "openocd" makefile target to kick open a GDB server.
# Use "localhost:4242" for texane's st-link utility (st-util) GDB server.
target extended-remote localhost:3333

# Spawn an OpenOCD GDB Server automatically and communicate to it via pipes.
# This method doesn't appear to work with semihosting (messages don't appear).
#target remote | openocd -f board/stm32f7discovery.cfg -c "gdb_port pipe;"

# The STM32F7-DISCO board only supports these amounts of breakpoints/watchpoints.
set remote hardware-breakpoint-limit 8
set remote hardware-watchpoint-limit 4

# Enable semihosting support (let printf() print out to the debug console).
monitor arm semihosting enable

# Reset and halt the CPU.
monitor reset halt

# Load the binary into flash.
load

# Set a breakpoint on the "run()" function.
b run

# Run to that breakpoint (aka, run past the reset handler).
continue

# Enable TUI (Text UI) mode because I find it easier to use.
tui enable