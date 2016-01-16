#include <stdio.h>

void kPrint(const int pos, const char* str) {
    char* videoAddr = (char*)(0xb8000 + pos * 2);
    while (*str != 0) {
        *(videoAddr++) = *str++;
        *(videoAddr++) = 0x04;
    }
}

void kClean(const int pos, const int size) {
    char* videoAddr = (char*)(0xb8000 + pos * 2);
    for (int i = 0; i < size; i++) {
        *(videoAddr++) = ' ';
        *(videoAddr++) = 0x04;
    }
}
