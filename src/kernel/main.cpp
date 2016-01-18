#include <bpb.h>
#include <stdio.h>

const gpt_status* gpt = (gpt_status*)0x0600;
const bpb_fat32*  bpb = (bpb_fat32*)0x7c00;

void main()
{
    kClean(0, 80 * 25);
    kPrint(0, "Hello C++ Kernel!!");

    while (1);
}
