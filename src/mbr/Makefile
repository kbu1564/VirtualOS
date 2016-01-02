boot: start.asm
	nasm start.asm -o Bootloader.bin
	sudo dd if=Bootloader.bin of=$(DRIVE_DEV) bs=512
clean:
	rm *.bin

