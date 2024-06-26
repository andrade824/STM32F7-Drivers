# You can override these settings by passing them in as arguments
PLATFORM ?= stm32f7
CONFIG ?= stm32f7_dev_board
#CONFIG ?= stm32f746_disco

# The device that JLink tries connecting to.
JLINK_DEVICE ?= STM32F730R8

# The GDB port that the JLink GDB server will expose.
JLINK_PORT ?= 2331

# Put your source files here (or *.c, etc)
SRCS += platform/*.c
SRCS += platform/$(PLATFORM)/*.S
SRCS += platform/$(PLATFORM)/*.c
SRCS += drivers/*.c
SRCS += drivers/spi/*.c
SRCS += drivers/$(PLATFORM)/*.c
SRCS += apps/*.c
SRCS += os/*.c

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME ?= hello_world

# Where to store the compilation outputs
OUTPUT_DIR ?= output

# Path where the project files will be built
PROJ_PATH = $(OUTPUT_DIR)/$(PROJ_NAME)

# Do you want semihosting in debug builds? "yes"/"no"
SEMIHOSTING_SUPPORT = yes

###############################################################################
CC=arm-none-eabi-gcc
DBG=arm-none-eabi-gdb-py
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -Wall -Wextra -Werror -Tplatform/$(PLATFORM)/linker-$(CONFIG).ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m7 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv5-sp-d16
CFLAGS += -I. -Iconfigs/ -Iplatform/ -Iplatform/$(PLATFORM)/ -Idrivers/$(PLATFORM)/ -Idrivers/

# Both Release/Debug builds should have debug information in the ELF file and
# use the nano version of newlib (removes features but saves space).
CFLAGS += -g3 --specs=nano.specs

# Extra warnings to enable that -Wall -Wextra don't enable.
CFLAGS += -Wshadow -Wdouble-promotion -Wformat=2 -Wformat-truncation=2 -fno-common

# Place every function in it's own section, and prune unused sections (saves lots of TEXT space).
CFLAGS += -ffunction-sections -Wl,--gc-sections #-Wl,--print-gc-sections

# Include files from CMSIS
CFLAGS += -Iplatform/CMSIS/Include

# Create defines for the CONFIG and PLATFORM
CFLAGS += -Dplatform_$(PLATFORM) -Dconfig_$(CONFIG)

# Extra C preprocessor flags that can be passed in the make invocation.
CFLAGS += $(CPPFLAGS)

# Conditionally enable semihosting. Disable if your debugger doesn't support it.
ifeq ($(SEMIHOSTING_SUPPORT), yes)
CFLAGS_SEMIHOSTING = --specs=rdimon.specs -DSEMIHOSTING_ENABLED
else
CFLAGS_SEMIHOSTING = --specs=nosys.specs
endif

# Potentially use semihosting and enable debug features when in debug mode. Only
# perform optimizations that don't disturb debugging.
#
# Keeping the frame pointer is needed to properly construct a backtrace when
# exceptions occur (due to the handwritten assembly for dumping registers).
CFLAGS_DEBUG = -Og -fno-omit-frame-pointer $(CFLAGS_SEMIHOSTING) -Wl,-Map,$(PROJ_PATH)-dbg.map -DDEBUG_ON

# No semihosting or debug features in release mode. Use better optimizations.
CFLAGS_RELEASE = -O2 --specs=nosys.specs -Wl,-Map,$(PROJ_PATH).map

OBJS = $(SRCS:.c=.o)

.PHONY: release debug clean jlink openocd burn_jlink burn_openocd size

all: release debug

release: $(PROJ_NAME).elf

debug: $(PROJ_NAME)-dbg.elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) $^ -o $(OUTPUT_DIR)/$@
	$(OBJCOPY) -O ihex $(PROJ_PATH).elf $(PROJ_PATH).hex
	$(OBJCOPY) -O binary $(PROJ_PATH).elf $(PROJ_PATH).bin

$(PROJ_NAME)-dbg.elf: $(SRCS)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $^ -o $(OUTPUT_DIR)/$@
	$(OBJCOPY) -O ihex $(PROJ_PATH)-dbg.elf $(PROJ_PATH)-dbg.hex
	$(OBJCOPY) -O binary $(PROJ_PATH)-dbg.elf $(PROJ_PATH)-dbg.bin

clean:
	rm -f $(OUTPUT_DIR)/*.o $(PROJ_PATH).elf $(PROJ_PATH).hex $(PROJ_PATH).bin \
		$(PROJ_PATH).map $(PROJ_PATH)-dbg.elf $(PROJ_PATH)-dbg.hex \
		$(PROJ_PATH)-dbg.bin

# Kick open GDB to debug an executable with debug symbols.
gdb_openocd: debug
	$(DBG) -x .gdbinit_openocd $(PROJ_PATH)-dbg.elf

gdb_jlink: debug
	$(DBG) -ex "target extended-remote localhost:$(JLINK_PORT)" -x .gdbinit_jlink $(PROJ_PATH)-dbg.elf

# Start an OpenOCD GDB server.
#
# By default, OpenOCD requires "sudo" to access USB devices. You'll probably
# want to add yourself to the 'plugdev' group and then add a udev rule for the
# ST-Link programmer. In the OpenOCD source code, 'contrib/60-openocd.rules'
# contains the correct rule for the ST-Link device that ensures the USB device
# file has the 'plugdev' group.
openocd:
	openocd -f board/stm32f7discovery.cfg

# Start a JLink GDB server. JLINK_PORT can be set during the make invocation
# like so: make jlink JLINK_PORT=2332. The port defaults to 2331.
jlink:
	JLinkGDBServer -device $(JLINK_DEVICE) -if SWD -speed 4000 -port $(JLINK_PORT)

# Flash the STM32F7 with the "release" executable.
burn_openocd: release
	openocd -f board/stm32f7discovery.cfg -c "program $(PROJ_PATH).elf reset exit"

burn_openocd_debug: debug
	openocd -f board/stm32f7discovery.cfg -c "program $(PROJ_PATH)-dbg.elf reset exit"

# Flash the STM32F7 with the "release" executable.
burn_jlink: release
	printf "r\nloadbin %s 0x08000000\nr\ngo\nexit\n" "$(PROJ_PATH).bin" > .jlink_command
	JLinkExe -device $(JLINK_DEVICE) -if SWD -speed 4000 -CommandFile .jlink_command

# Print out the size of the text, data, and bss regions for any built
# executables.
size:
	arm-none-eabi-size $(PROJ_PATH)*.elf
