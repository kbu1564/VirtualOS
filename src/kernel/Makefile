all: main.o kernel

main.o:
	$(CXX32)   -c -ffreestanding -I./include stdio.cpp main.cpp
	$(LD)      -melf_i386 -T main.ld -nostdlib main.o stdio.o -o main.elf
	$(OBJCOPY) -j .text -j .data -j .rodata -j .bss -S -O binary main.elf main.o

kernel:
	cp main.o $(DRIVE_DIR)

clean:
	rm *.o
	rm *.elf
