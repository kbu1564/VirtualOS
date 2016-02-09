#ifndef _FS_GPT_H_
#define _FS_GPT_H_

// gpt entry
struct gpt_entry {
    u8  bootIndicator;
    union {
        struct {
            u8  startingHead;
            u8  startingSector : 6;
            u16 startCylinder  : 10;
            u8  systemID;
            u8  endingHead;
            u8  endingSector   : 6;
            u16 endingCylinder : 10;
            u32 relativeSector;
            u32 totalSectors;
        } __attribute__((packed)) chs;

        struct {
            u8  signature1;
            u16 partitionStartHigh;
            u8  systemID;
            u8  signature2;
            u16 partitionLengthHigh;
            u32 partitionStartLow;
            u32 partitionLengthLow;
        } __attribute__((packed)) lba;
    } type;
} __attribute__((packed));

// gpt parameter
struct gpt_param {
    u8               byteCode[446];
    struct gpt_entry gpts[4];
    u16              mbrSignature;
} __attribute__((packed));

#endif

