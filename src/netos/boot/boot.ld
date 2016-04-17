OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(main)
SECTIONS
{
    . = 0x7c00;
    .text : AT(0x7c00) { *(.text) }

    . = ALIGN(16);
    .rodata : { *(.rodata*) }

    . = ALIGN(16);
    .data : { *(.data*) }

    .signature : AT(0x7dfe)
    {
        setup_sig = .;
        SHORT(0xaa55)
    }
    . = ALIGN(16);
    _end = .;

    /DISCARD/ : { *(.note*) }

    . = ASSERT(_end <= 0x8000, "Setup too big!");
}

