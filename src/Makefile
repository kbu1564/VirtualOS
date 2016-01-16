CROSS_COMPILE_DIR := /usr/local/cross/bin
PREFIX := x86_64-pc-linux-

CC      := $(CROSS_COMPILE_DIR)/$(PREFIX)gcc
CXX     := $(CROSS_COMPILE_DIR)/$(PREFIX)g++
LD      := $(CROSS_COMPILE_DIR)/$(PREFIX)ld
OBJCOPY := $(CROSS_COMPILE_DIR)/$(PREFIX)objcopy
CC32    := $(CC) -m32
CXX32   := $(CXX) -m32

#####################################################################
# Makefile 을 이용한 환경 셋팅값 적용 부분
#####################################################################
Q	:= @
SUDO	:= sudo

EMPTY = 
ifeq ($(DRIVE_NAME), $(EMPTY))
    all: need_parameter

    need_parameter:
		$(Q) echo "Networking Embedded Operating System NetOS (@kbu1564) (http://github.com/kbu1564/NetOS.git)"
		$(Q) echo "LICENSE : MIT"
		$(Q) echo ""
		$(Q) echo "Useage : make DRIVE_NAME=input_your_usb_drive_label_name [all|run]"
		$(Q) echo "example : make DRIVE_NAME=OSUSB [all|run]"
else
    all: build

    # 운영체제 버전별 명령어 셋팅
    OS_DETECTING     = $(shell uname -s)
    ifneq ($(OS_DETECTING), Linux)
	SUDO := 
    endif

    DRIVE_DEV       := $(shell mount | grep $(DRIVE_NAME) | awk '{split($$0,arr," "); print arr[1];}')
    DRIVE_DEV       := $(subst /dev/disk,/dev/rdisk,$(DRIVE_DEV))
    DRIVE_DIR       := $(shell mount | grep $(DRIVE_NAME) | awk '{split($$0,arr," "); print arr[3];}')
    MAKEOPTS        := CC="$(CC)" CXX="$(CXX)" LD="$(LD)" OBJCOPY="$(OBJCOPY)" CC32="$(CC32)" CXX32="$(CXX32)" DRIVE_DEV="$(DRIVE_DEV)" DRIVE_DIR="$(DRIVE_DIR)" SUDO="$(SUDO)"

    build:
		make $(MAKEOPTS) -C ./mbr
		make $(MAKEOPTS) -C ./loader
		make $(MAKEOPTS) -C ./kernel
		./mbr/install/install -device-path=$(DRIVE_DEV) -mbr-path=./mbr/mbr.bin -vbr-path=./mbr/vbr.bin
           
    run:
		$(SUDO) qemu-system-x86_64 -m 64 -hda $(DRIVE_DEV)
           
    clean:
		make $(MAKEOPTS) -C ./mbr    clean
		make $(MAKEOPTS) -C ./loader clean
		make $(MAKEOPTS) -C ./kernel clean
endif
