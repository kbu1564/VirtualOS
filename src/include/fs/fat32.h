#ifndef _BOOT_FAT32_H_
#define _BOOT_FAT32_H_

struct fat32_param {
    u8   jmpCode[3];
    char oemID[8];
    u16  bytesPerSector;
    u8   sectorsPerCluster;
    u16  reservedSectors;
    u8   totalFATs;
    u16  maxRootEntries;
    u16  numberOfSectors;
    u8   mediaDescriptor;
    u16  sectorsPerFAT;
    u16  sectorsPerTrack;
    u16  sectorsPerHead;
    u32  hiddenSectors;
    u32  totalSectors;
    u32  bigSectorsPerFAT;
    u16  flags;
    u16  fsVersion;
    u32  rootDirectoryStart;
    u16  fsInfoSector;
    u16  backupBootSector;

    u32  reserved1;
    u32  reserved2;
    u32  reserved3;

    u8   bootDiskNumber;
    u8   reserved4;
    u8   signature;
    u32  volumeID;
    char volumeLabel[11];
    char systemID[8];

    u8   byteCode[420];
    u16  vbrSignature;
} __attribute__((packed));

#endif

