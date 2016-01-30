#ifndef _FS_GPT_H_
#define _FS_GPT_H_

typedef struct {
    unsigned char  BootIndicator;
    union {
        struct {
            unsigned char  StartingHead;
            unsigned short StartingSector : 6;
            unsigned short StartCylinder  : 10;
            unsigned char  SystemID;
            unsigned char  EndingHead;
            unsigned short EndingSector   : 6;
            unsigned short EndingCylinder : 10;
            unsigned int   RelativeSector;
            unsigned int   totalSectors;
        } __attribute__((packed)) chs;

        struct {
            unsigned char  Signature1;
            unsigned short PartitionStartHigh;
            unsigned char  SystemID;
            unsigned char  Signature2;
            unsigned short PartitionLengthHigh;
            unsigned int   PartitionStartLow;
            unsigned int   PartitionLengthLow;
        } __attribute__((packed)) lba;
    } type;
} __attribute__((packed)) gpt_entry;

typedef struct {
    unsigned char  byteCode[446];
    gpt_entry      gpt[4];
    unsigned short MBRSignature;
} __attribute__((packed)) gpt_status;

#endif

