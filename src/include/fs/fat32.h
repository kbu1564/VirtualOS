#ifndef _BOOT_FAT32_H_
#define _BOOT_FAT32_H_

typedef struct {
    unsigned char  JumpCode[3];
    unsigned char  OemID[8];
    unsigned short BytesPerSector;
    unsigned char  SectorsPerCluster;
    unsigned short ReservedSectors;
    unsigned char  TotalFATs;
    unsigned short MaxRootEntries;
    unsigned short NumberOfSectors;
    unsigned char  MediaDescriptor;
    unsigned short SectorsPerFAT;
    unsigned short SectorsPerTrack;
    unsigned short SectorsPerHead;
    unsigned int   HiddenSectors;
    unsigned int   TotalSectors;
    unsigned int   BigSectorsPerFAT;
    unsigned short Flags;
    unsigned short FSVersion;
    unsigned int   RootDirectoryStart;
    unsigned short FSInfoSector;
    unsigned short BackupBootSector;

    unsigned int   Reserved1;
    unsigned int   Reserved2;
    unsigned int   Reserved3;

    unsigned char  BootDiskNumber;
    unsigned char  Reserved4;
    unsigned char  Signature;
    unsigned int   VolumeID;
    unsigned char  VolumeLabel[11];
    unsigned char  SystemID[8];

    unsigned char  byteCode[420];
    unsigned short VBRSignature;
} __attribute__((packed)) bpb_fat32;

#endif

