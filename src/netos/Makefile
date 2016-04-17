CROSS_COMPILE_DIR := /usr/local/cross/bin
PREFIX := x86_64-pc-linux-

CC      := $(CROSS_COMPILE_DIR)/$(PREFIX)gcc
CXX     := $(CROSS_COMPILE_DIR)/$(PREFIX)g++
LD      := $(CROSS_COMPILE_DIR)/$(PREFIX)ld
OBJCOPY := $(CROSS_COMPILE_DIR)/$(PREFIX)objcopy
CC32    := $(CC) -m32
CXX32   := $(CXX) -m32 -std=c++11

#####################################################################
# Makefile 을 이용한 환경 셋팅값 적용 부분
#####################################################################
Q	:= @
SUDO	:= sudo

EMPTY = 
ifeq ($(DRIVE_NAME), $(EMPTY))
all: welcome help
else
all: build
endif

# 운영체제 버전별 명령어 셋팅
OS_DETECTING = $(shell uname -s)
ifneq ($(OS_DETECTING), Linux)
SUDO := 
endif

PATTN     := split($$0,arr," ");
DRIVE_DEV := $(shell mount | grep '$(DRIVE_NAME)' | awk '{$(PATTN)print arr[1];}')
DRIVE_DEV := $(subst /dev/disk,/dev/rdisk,$(DRIVE_DEV))
DRIVE_DIR := $(shell mount | grep '$(DRIVE_NAME)' | awk '{$(PATTN)print arr[3];}')

MAKEOPTS  := Q="$(Q)" CC="$(CC)" CXX="$(CXX)" LD="$(LD)" OBJCOPY="$(OBJCOPY)" CC32="$(CC32)" CXX32="$(CXX32)" DRIVE_DEV="$(DRIVE_DEV)" DRIVE_DIR="$(DRIVE_DIR)" SUDO="$(SUDO)"

welcome:
	$(Q) echo "Networking Embedded Operating System NetOS (@kbu1564)"
	$(Q) echo "Github: http://github.com/kbu1564/NetOS.git"

help:
	$(Q) echo "Useage:"
	$(Q) echo "  make DRIVE_NAME=(usb_label_name) [all|run]"
	$(Q) echo "example:"
	$(Q) echo "  make DRIVE_NAME=OSUSB [all|run]"

bootwriter:
	$(Q) make $(MAKEOPTS) -C ./boot/install
	./boot/install/install -device-path=$(DRIVE_DEV) -mbr-path=./boot/mbr.bin -vbr-path=./boot/vbr.bin

build:
	$(Q) make $(MAKEOPTS) -C ./boot
	$(Q) make $(MAKEOPTS) -C ./kernel

run:
	$(SUDO) qemu-system-x86_64 -m 64 -hda $(DRIVE_DEV) -M pc

clean:
	$(Q) make $(MAKEOPTS) -C ./boot clean
	$(Q) make $(MAKEOPTS) -C ./kernel clean
