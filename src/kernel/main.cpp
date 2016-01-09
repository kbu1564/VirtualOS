extern "C" void kPrint32(int pos, const char* str);
void main()
{
    kPrint32(80 * 2 * 4, "Hello C++ Kernel!!");
    while (1);
}

