#include <types.h>
#include <stdio.h>

unsigned char g_color        = 0x04;
char*         g_videoPointer = (char*)0xb8000;

void puts(const char* str) {
    char c;
    while ((c = *str++) != '\0') {
        *(g_videoPointer++) = c;
        *(g_videoPointer++) = g_color;
    }
}

