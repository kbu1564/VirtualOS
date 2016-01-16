MAKEOPTS := CC="$(CC)" CXX="$(CXX)" LD="$(LD)" OBJCOPY="$(OBJCOPY)" CC32="$(CC32)" CXX32="$(CXX32)" DRIVE_DEV="$(DRIVE_DEV)" DRIVE_DIR="$(DRIVE_DIR)" SUDO="$(SUDO)"

all: build

build: mbr.bin vbr.bin
	make $(MAKEOPTS) -C ./install

vbr.bin: vbr.asm
	nasm vbr.asm -o vbr.bin

mbr.bin: mbr.asm
	nasm mbr.asm -o mbr.bin

clean:
	rm *.bin
	make $(MAKEOPTS) -C ./install clean
