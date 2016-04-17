static void put(const char c) { asm volatile ("int $0x10" : : "a"(0x0e00 | c), "b"(0x0007)); }
void main() {
  put('H'); put('i');
  while (1);
}
