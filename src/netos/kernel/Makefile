all: kernel.o install

CXXFLAGS = -m16 -c -ffreestanding
LDFLAGS = -melf_i386 -nostdlib
INCLUDE_PATH = ../../include

kernel.o:
	$(CXX32) $(CXXFLAGS) -I$(INCLUDE_PATH) ../lib/*.cpp
	$(CXX32) $(CXXFLAGS) -I$(INCLUDE_PATH) *.cpp
	$(LD) $(LDFLAGS) -T main.ld main.o stdio.o -o main.elf
	$(OBJCOPY) -j .text -j .data -j .rodata -j .bss -S -O binary main.elf kernel.o

install:
	cp kernel.o $(DRIVE_DIR)

clean:
	rm *.o
	rm *.elf
