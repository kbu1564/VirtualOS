all: build

build: mbr.bin vbr.bin

vbr.bin: vbr.asm
	nasm vbr.asm -o vbr.bin

mbr.bin: mbr.asm
	nasm mbr.asm -o mbr.bin

clean:
	rm *.bin
