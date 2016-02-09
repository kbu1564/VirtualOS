OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(main)
SECTIONS
{
    .text 0xA000:
    {
        *(.text .text.* .gnu.linkonce.t.*)
    }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
    .data :
    {
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    }
}

