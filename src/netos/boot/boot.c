struct regs {
  long edi, esi;
  long ebp, esp;
  long ebx, edx;
  long ecx, eax;
};

/**
 * interrupt call
 */
static void intcall(const int callno, struct regs& regs) {
  asm volatile (
    "int %0"
    :
    : "i"(callno), "a"(regs.eax), "b"(regs.ebx), "c"(regs.ecx), "d"(regs.edx), "S"(regs.esi), "D"(regs.edi)
  );
}

/**
 * print character
 */
static void put(const char c) {
  struct regs regs;

  regs.eax = 0x0e00 | c;
  regs.ebx = 0x0007;
  intcall(0x10, regs);
}

/**
 * print string
 */
static void puts(const char* str) {
  while(*str)
    put(*str++);
}

/**
 * bootloader main function
 */
void main() {
  puts("Hi NetOS.git");
  while (1);
}
