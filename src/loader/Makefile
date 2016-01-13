all: loader.o loader

loader.o: loader.asm
	nasm loader.asm -o loader.o

loader:
	cp loader.o $(DRIVE_DIR)

clean:
	rm *.o

