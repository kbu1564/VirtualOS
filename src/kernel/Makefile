all: main.o kernel

main.o:
	$(CC32)    -c -ffreestanding main.c
	$(LD)      -melf_i386 -T main.ld -nostdlib main.o ../loader/print32.o -o main.elf
	$(OBJCOPY) -j .text -j .data -j .rodata -j .bss -S -O binary main.elf main.o

kernel:
	cp main.o $(DRIVE_DIR)
clean:
	rm *.o
	rm *.elf

