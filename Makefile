# Put your source files here (or *.c, etc)
SRCS  = platform/*.s
SRCS += platform/*.c
SRCS += drivers/*.c
SRCS += apps/*.c

# Include the drivers you want enabled
DRIVERS = -DINCLUDE_SDRAM_DRIVER -DINCLUDE_LCD_CTRL_DRIVER -DINCLUDE_DMA2D_DRIVER

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME = hello_world

# Where to store the compilation outputs
OUTPUT_DIR = output

# Path where the project files will be built
PROJ_PATH =  $(OUTPUT_DIR)/$(PROJ_NAME)

# Do you want semihosting in debug builds? "yes"/"no"
SEMIHOSTING_SUPPORT = yes

###############################################################################
#TOOLCHAIN=/home/devon/toolchains/gcc-arm-none-eabi-6_2-2016q4/bin
CC=arm-none-eabi-gcc
DBG=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -Wall -Wextra -Werror -Tplatform/stm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m7 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv5-sp-d16
CFLAGS += -Iplatform/ -Idrivers/ -Iapps/

# Include files from CMSIS
CFLAGS += -Iplatform/CMSIS/Include

# Include any needed drivers
CFLAGS += $(DRIVERS)

# Conditionally enable semihosting. Disable if your debugger doesn't support it.
ifeq ($(SEMIHOSTING_SUPPORT), yes)
CFLAGS_SEMIHOSTING = --specs=rdimon.specs -lc -lrdimon
else
CFLAGS_SEMIHOSTING = --specs=nosys.specs
endif

# Use semihosting and debug features when in debug mode. Only perform
# optimizations that don't disturb debugging.
CFLAGS_DEBUG = -Og -g3 $(CFLAGS_SEMIHOSTING) -DDEBUG_ON

# No semihosting or debug features in release mode. Use better optimizations.
CFLAGS_RELEASE = -O2 --specs=nosys.specs -Wl,-Map,$(PROJ_PATH).map

OBJS = $(SRCS:.c=.o)

.PHONY: release debug clean gdb openocd burn size

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
gdb: debug
	$(DBG) $(PROJ_PATH)-dbg.elf

# Start an OpenOCD GDB server.
#
# By default, OpenOCD requires "sudo" to access USB devices. You'll probably
# want to add yourself to the 'plugdev' group and then add a udev rule for the
# ST-Link programmer. In the OpenOCD source code, 'contrib/60-openocd.rules'
# contains the correct rule for the ST-Link device that ensures the USB device
# file has the 'plugdev' group.
openocd: debug
	openocd -f board/stm32f7discovery.cfg

# Flash the STM32F7 with the "release" executable.
burn: release
	openocd -f board/stm32f7discovery.cfg -c "program $(PROJ_PATH).elf reset exit"

# Print out the size of the text, data, and bss regions for any built
# executables.
size:
	arm-none-eabi-size $(PROJ_PATH)*.elf
