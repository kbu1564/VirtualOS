all: install

CXXFLAGS = -c -g -O2 -m16 -ffreestanding -fno-stack-protector
LDFLAGS = -melf_i386 -static -nmagic -nostdlib
INCLUDE_PATH = ../../include

boot.elf: boot.c boot.ld
	$(CXX32) $(CXXFLAGS) -I$(INCLUDE_PATH) boot.c
	$(LD) $(LDFLAGS) -T boot.ld boot.o -o boot.elf

install: boot.elf
	cp boot.elf $(DRIVE_DIR)

clean:
	rm *.o
	rm *.elf
