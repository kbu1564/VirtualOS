#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
using namespace std;

typedef struct { bool active, need; } command_status;
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

#define COMMAND_DEVICE_NAME  "device-path"
#define COMMAND_MBR_NAME     "mbr-path"
#define COMMAND_VBR_NAME     "vbr-path"

// supported command list
// key : { active, need }
map< string, command_status > comlist = {
    { COMMAND_DEVICE_NAME, { true, true } },
    { COMMAND_MBR_NAME,    { true, true } },
    { COMMAND_VBR_NAME,    { true, true } },
};

// standard
// CommandLine으로 넘어온 옵션값을 파싱하여 map object로 리턴
map<string, string> parseOption(int argc, char* argv[]) {
    map<string, string> opts;
    for (int i = 1; i < argc; i++) {
        string optValue = argv[i] + 1;
        size_t idx = optValue.find('=');
        if (idx != string::npos) {
            opts.insert(pair<string, string>(optValue.substr(0, idx), optValue.substr(idx + 1, -1)));
        }
    }
    return opts;
}

// Windows 전용
// 물리 디스크의 번호를 얻는다.
int getPhysicalDriveNumber(const char* driveName) {
    VOLUME_DISK_EXTENTS pstVolumeData;
    char devicePath[40];
    sprintf(devicePath, "\\\\.\\%s:", driveName);

    // device open
    HANDLE hDevice = CreateFile(devicePath, 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    // device open error
    if (hDevice == INVALID_HANDLE_VALUE)
        return -1;

    DWORD dwOut;
    BOOL result = DeviceIoControl(hDevice, 
        IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
        NULL, 
        0, 
        &pstVolumeData, 
        sizeof(pstVolumeData), 
        &dwOut, 
        NULL
    );

    if (result == FALSE || pstVolumeData.NumberOfDiskExtents < 1)
        return -1;

    // device close
    CloseHandle(hDevice);

    return pstVolumeData.Extents[0].DiskNumber;
}

// Windows 전용
// 특정 파일의 내용을 구하는 함수
BYTE* ReadFileContents(const char* filename) {
    BYTE* data = new BYTE[512];
    DWORD dwRead = 0;
    // device open
    HANDLE hDevice = CreateFile(filename, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    // file open error
    if (hDevice == INVALID_HANDLE_VALUE) {
        cout << "File Opening ErrorCode : " << GetLastError() << endl;
        delete[] data;
        return nullptr;
    }

    BOOL result = ReadFile(hDevice, data, 512, &dwRead, NULL);
    if (result != INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        delete[] data;
        return nullptr;
    }
    CloseHandle(hDevice);

    return data;
}

// Windows 전용
// 특정 섹터를 읽어들이는 함수
BYTE* ReadSector(HANDLE hDevice, const int nStartSector, const int nSectorCount) {
    BYTE* data = new BYTE[nSectorCount * 512];
    BOOL result = FALSE;
    DWORD dwRead = 0;
    DWORD dwLow = nStartSector * 512;
    result = SetFilePointer(hDevice, dwLow, NULL, FILE_BEGIN);
    if (result != INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        delete[] data;
        return nullptr;
    }
    result = ReadFile(hDevice, data, nSectorCount * 512, &dwRead, NULL);
    if (result != INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        delete[] data;
        return nullptr;
    }
    return data;
}

// Windows 전용
// 특정 섹터에 데이터를 작성하는 함수
bool WriteSector(HANDLE hDevice, const int nStartSector, BYTE* data, int dataLength) {
    BOOL result = FALSE;
    DWORD dwRead = 0, dwWrite = 0;
    DWORD dwLow = nStartSector * 512;
    result = SetFilePointer(hDevice, dwLow, NULL, FILE_BEGIN);
    if (result != INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
        return false;

    result = WriteFile(hDevice, data, dataLength, &dwWrite, NULL);
    if (result != INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
        return false;

    return true;
}

int main(int argc, char* argv[]) {
    map<string, string> opts = parseOption(argc, argv);
    // 옵션 상태값 체크
    cout << "<< Option Status >>" << endl;
    for (auto iter = opts.begin(); iter != opts.end(); iter++) {
        // check : not supported options
        if (comlist[iter->first].active == false) {
            cout << "'" << iter->first << "' option is not supported!!" << endl;
            return -1;
        }
        cout << iter->first << ": " << iter->second << endl;
    }
    cout << endl;

    // 필수 옵션 값 존재여부 체크
    for (auto iter = comlist.begin(); iter != comlist.end(); iter++) {
        if (iter->second.need == true && opts[iter->first] == "") {
            cout << "'" << iter->first << "' option is not exists!!" << endl;
            return -1;
        }
    }

    // MBR or VBR 에서 기계어 코드부분 추출
    gpt_status* mbrArea = (gpt_status*)ReadFileContents(opts[COMMAND_MBR_NAME].c_str());
    bpb_fat32* vbrArea = (bpb_fat32*)ReadFileContents(opts[COMMAND_VBR_NAME].c_str());
    if (mbrArea == nullptr || vbrArea == nullptr) {
        cout << "MBR or VBR was analysis failure!!" << endl;
        return -1;
    }
    if (mbrArea->MBRSignature != 0xAA55 || vbrArea->VBRSignature != 0xAA55) {
        cout << "MBR or VBR was analysis failure!!" << endl;
        return -1;
    }
    // MBR or VBR BootCode : 'mbrArea->byteCode' or 'vbrArea->byteCode'

    // Save MBR or VBR bootCode
    char devicePath[40];
    int physicalNumber = getPhysicalDriveNumber(opts[COMMAND_DEVICE_NAME].c_str());
    if (physicalNumber < 0)
        return -1;

    // get physical drive handle
    sprintf(devicePath, "\\\\.\\PhysicalDrive%d", physicalNumber);

    HANDLE hDevice = CreateFile(devicePath, 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        NULL, 
        OPEN_EXISTING, 
        0, 
        NULL
    );
    // device open error
    if (hDevice == INVALID_HANDLE_VALUE) {
        cout << "Device Opening ErrorCode : " << GetLastError() << endl;
        return -1;
    }

    // get 0 sector
    BYTE* sectorMBR = ReadSector(hDevice, 0, 1);
    if (sectorMBR != nullptr) {
        // get Bios Parameter Block
        bpb_fat32* bpb = (bpb_fat32*)sectorMBR;
        if (bpb->JumpCode[2] == 0x90 && bpb->VBRSignature == 0xAA55) {
            // MBR
            char systemID[9] = { 0, };
            strncpy(systemID, (char*)bpb->SystemID, 8);
            cout << "<< MBR(" << sizeof(bpb_fat32) << ") Entry >>" << endl;

            printf("VolumeID   : 0x%08X\n", bpb->VolumeID);
            printf("SystemID   : %s\n", systemID);
            printf("DeviceSize : %.02fGB\n", bpb->TotalSectors / 1024.0 * bpb->BytesPerSector / 1024.0 / 1024.0);

            //--------------------------------------------------------------------------------
            // Write to MBR BootCode
            //--------------------------------------------------------------------------------
            cout << "<< Change BootCode(" << sizeof(bpb->byteCode) << ") >>" << endl;
            // bootCode Copy
            memcpy(bpb->byteCode, vbrArea->byteCode, sizeof(bpb->byteCode));
            // write to ByteCode
            if (WriteSector(hDevice, 0, (BYTE*)bpb, sizeof(bpb_fat32)) == false) {
                cout << "Write MBR BootSector: Error(" << GetLastError() << ")" << endl;
            } else {
                cout << "Write MBR BootSector: Success!!" << endl;
            }
            //--------------------------------------------------------------------------------
        } else {
            // GPT
            gpt_status* gpt = (gpt_status*)sectorMBR;
            gpt_entry* gptEntry = gpt->gpt;
            // bootCode Copy
            memcpy(gpt->byteCode, mbrArea->byteCode, sizeof(gpt->byteCode));
            
            cout << "<< GPT Entry(" << sizeof(gpt_status) << ") >>" << endl << endl;

            //--------------------------------------------------------------------------------
            // Write to GPT BootCode
            //--------------------------------------------------------------------------------
            cout << "<< Change GPT ByteCode(" << sizeof(gpt->byteCode) << ") >>" << endl;
            if (WriteSector(hDevice, 0, (BYTE*)gpt, sizeof(gpt_status)) == false) {
                cout << "Write GPT BootSector: Error(" << GetLastError() << ")" << endl;
            } else {
                cout << "Write GPT BootSector: Success!!" << endl;
            }
            cout << endl;
            //--------------------------------------------------------------------------------

            for (int i = 0; i < 4; i++) {
                // bootable
                printf("Partition Entry #%d Bootable : 0x%02X\n", i + 1, (short)gptEntry[i].BootIndicator);

                // check chs or lba type disk
                if ((gptEntry[i].BootIndicator & 1) == 0) {
                    // CHS type
                    printf("Partition Entry #%d Type     : CHS\n", i + 1);
                    printf("Partition Entry #%d Start    : 0x%02X\n", i + 1, (short)gptEntry[i].type.chs.RelativeSector);

                    // read volume boot record
                    if ((short)gptEntry[i].type.chs.StartingSector > 0) {
                        // move partition master boot record
                        BYTE* sectorVBR = ReadSector(hDevice, (int)gptEntry[i].type.chs.RelativeSector, 512);
                        if (sectorVBR == nullptr)
                            break;

                        bpb = (bpb_fat32*)sectorVBR;
                        if (bpb->JumpCode[2] == 0x90 && bpb->VBRSignature == 0xAA55) {
                            char systemID[9] = { 0, };
                            strncpy(systemID, (char*)bpb->SystemID, 8);
                            cout << "  << Partition #" << i + 1 << " VBR(" << sizeof(bpb_fat32) << ") Entry >>" << endl;
                            // MBR
                            printf("  Partition Entry #%d VolumeID   : 0x%08X\n", i + 1, bpb->VolumeID);
                            printf("  Partition Entry #%d SystemID   : %s\n", i + 1, systemID);
                            printf("  Partition Entry #%d DeviceSize : %.02fGB\n", i + 1, (bpb->TotalSectors / 1024.0 * bpb->BytesPerSector / 1024.0 / 1024.0));

                            //--------------------------------------------------------------------------------
                            // Write to MBR BootCode
                            //--------------------------------------------------------------------------------
                            cout << "  << Change BootCode(" << sizeof(bpb->byteCode) << ") >>" << endl;
                            // bootCode Copy
                            memcpy(bpb->byteCode, vbrArea->byteCode, sizeof(bpb->byteCode));
                            // write to ByteCode
                            if (WriteSector(hDevice, (int)gptEntry[i].type.chs.RelativeSector, (BYTE*)bpb, sizeof(bpb_fat32)) == false) {
                                cout << "  Write MBR BootSector: Error(" << GetLastError() << ")" << endl;
                            } else {
                                cout << "  Write MBR BootSector: Success!!" << endl;
                            }
                            //--------------------------------------------------------------------------------
                        }
                        delete[] sectorVBR;
                    }
                } else {
                    // LBA type
                    printf("Partition Entry #%d Type  : LBA\n", i + 1);
                    printf("Partition Entry #%d Start : 0x%02X\n", i + 1, (short)gptEntry[i].type.lba.PartitionStartLow);
                }
                cout << endl;
            }
        }
        delete[] sectorMBR;
    }
    // device close
    CloseHandle(hDevice);
    
    return 0;
}
