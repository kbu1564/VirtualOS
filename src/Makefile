CROSS_COMPILE_DIR := /usr/local/cross/bin
PREFIX := x86_64-pc-linux-

CC      := $(CROSS_COMPILE_DIR)/$(PREFIX)gcc
CXX     := $(CROSS_COMPILE_DIR)/$(PREFIX)g++
LD      := $(CROSS_COMPILE_DIR)/$(PREFIX)ld
OBJCOPY := $(CROSS_COMPILE_DIR)/$(PREFIX)objcopy
CC32    := $(CC) -m32
CXX32   := $(CXX) -m32

DRIVE_DEV := $(shell mount | grep $(DRIVE_NAME) | awk '{split($$0,arr," "); print arr[1];}')
DRIVE_DEV := $(subst /dev/disk,/dev/rdisk,$(DRIVE_DEV))
DRIVE_DIR := $(shell mount | grep $(DRIVE_NAME) | awk '{split($$0,arr," "); print arr[3];}')
MAKEOPTS  := CC="$(CC)" CXX="$(CXX)" LD="$(LD)" OBJCOPY="$(OBJCOPY)" CC32="$(CC32)" CXX32="$(CXX32)" DRIVE_DEV="$(DRIVE_DEV)" DRIVE_DIR="$(DRIVE_DIR)"

build:
	make $(MAKEOPTS) -C ./mbr
	make $(MAKEOPTS) -C ./loader
	make $(MAKEOPTS) -C ./kernel

run:
	sudo qemu-system-x86_64 -m 64 -hda $(DRIVE_DEV)

clean:
	make $(MAKEOPTS) -C ./mbr    clean
	make $(MAKEOPTS) -C ./loader clean
	make $(MAKEOPTS) -C ./kernel clean
