all: loader.o print32.o loader

print32.o: print32.asm
	nasm -f elf32 $< -o $@
loader.o: loader.asm
	nasm loader.asm -o loader.o

loader:
	cp loader.o $(DRIVE_DIR)
clean:
	rm *.o

