#include <stdlib.h>

void* memcpy(void* dest, const void* src, size_t n) {
  u8* p_src  = (u8*)src;
  u8* p_dest = (u8*)dest;
  for (int i = 0; i < n; i++) *p_dest++ = *p_src++;
  return dest;
}

void* memset(void* src, const u8 data, size_t n) {
  u8* p_src = (u8*)src;
  for (int i = 0; i < n; i++) p_src++ = data;
  return src;
}

