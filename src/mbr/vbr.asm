[bits 16]
[org 0x7C00]

GPTAddress          equ 0x0600
RootDirectory       equ 0x8000
; 파일을 메모리에 올리게 되는 주소
LoadAddress         equ 0x9000

jmp main
nop

;-----------------------------------------
; File Allocation Table
;-----------------------------------------
OemID               db "HELLOOS "
BytesPerSector      dw 0x0200
SectorsPerCluster   db 0x08
ReservedSectors     dw 0x073E
TotalFATs           db 0x02
MaxRootEntries      dw 0x0000
NumberOfSectors     dw 0x0000
MediaDescriptor     db 0xF8
SectorsPerFAT       dw 0x0000
SectorsPerTrack     dw 0x003F
SectorsPerHead      dw 0x00FF
HiddenSectors       dd 0x00000000 ; GPT 지원을 위한 해당 MBR의 물리적 섹터의 위치
TotalSectors        dd 0x00F007FD
BigSectorsPerFAT    dd 0x00003C61
Flags               dw 0x0000
FSVersion           dw 0x0000
RootDirectoryStart  dd 0x00000002
FSInfoSector        dw 0x0001
BackupBootSector    dw 0x0006

Reserved1           dd 0
Reserved2           dd 0
Reserved3           dd 0

BootDiskNumber      db 0x80
Reserved4           db 0
Signature           db 0x29
VolumeID            dd 0x728158D5
VolumeLabel         db "OSUSB      "
SystemID            db "FAT32   "

;-----------------------------------------
; Bootloader Main Function
;-----------------------------------------
main:               xor ax, ax
                    mov es, ax
                    mov ds, ax
                    mov ss, ax
                    mov fs, ax
                    mov gs, ax
                    mov sp, 0xFFF8

.loader:            xor ecx, ecx
                    mov cl, byte [TotalFATs]
                    mov eax, dword [BigSectorsPerFAT]
                    mul ecx
                    ; dx:ax = ax * r16
                    ; edx:eax = eax * r32
                    xor ecx, ecx
                    mov cx, word [ReservedSectors]
                    add eax, ecx

                    cmp word [GPTAddress + 510], 0xAA55
                    jne .mbrboot

                    add eax, dword [HiddenSectors]

                    ; RootDir의 위치 : eax
.mbrboot:           mov dword [StartSector], eax

                    ; 디스크의 데이터를 읽어오기 위해 인터럽트를 호출한다.
                    mov ah, 0x42
                    mov dl, byte [BootDiskNumber]
                    mov si, DiskAddressPacket
                    int 0x13
                    jc .failed
                    ; RootDirEntry의 내용이 구해짐(폴더에 뭐있는지에 대한 목록이 구해짐)
                    ; 0x8000 -> loader.sys 파일을 찾아야되 -> 데이터가 저장된 위치를 찾고
                    ; -> 찾은 위치를 int 0x13 이용해서 0x8000번지 메모리에 적재 -> jmp 0x8000

                    ; 읽은 RootDirEntry로 부터 loader파일을 찾는다
                    mov di, RootDirectory

.find:              mov al, byte [di]
                    cmp al, 0xE5
                    je .next
                    ; 만약 첫번째 바이트가 0xE5 ?? => 삭제된 파일
                    ; 삭제된게 아닐경우 파일이름 비교
                    test al, al
                    jz .failed

                    ; 이 부분이 실행된다는 것은 삭제된 파일이 아니라는 것이다.
                    mov cx, 11
                    mov bx, LoaderName
                    mov si, di

.compare:           mov al, byte [bx]
                    cmp al, byte [si]
                    jne .next
                    ; dx의 1바이트가 si의 1바이트와 같으면 루프 계속 다르면 다음 파일정보 찾기

                    inc bx
                    inc si
                    loop .compare
                    ; for (int i = ??; i > 0; i--) { ... }
                    jmp .run

.next:              add di, 0x20
                    jmp .find

.run:               xor eax, eax
                    ; 이 시점에 위치한 상태인 경우 원하는 파일을 찾은 상태이다.
                    mov ax, word [di + 20]
                    shl eax, 16
                    mov ax, word [di + 26]
                    sub eax, dword [RootDirectoryStart]
                    ; 이때의 eax 값이 파일 정보가 저장된 클러스터 위치 값이다.
                    xor ecx, ecx
                    mov cl, byte [SectorsPerCluster]
                    mul ecx

                    add eax, dword [StartSector]

                    mov dword [StartSector], eax
                    mov word [DiskAddressPacket + 4], LoadAddress

                    ; 디스크의 데이터를 읽어오기 위해 인터럽트를 호출한다.
                    mov ah, 0x42
                    mov dl, byte [BootDiskNumber]
                    mov si, DiskAddressPacket
                    int 0x13
                    jc .failed

.success:           xor ax, ax
                    mov es, ax

                    jmp LoadAddress

.failed:            mov ax, 0xB800
                    mov es, ax

                    mov si, ErrorMsg
                    xor di, di
.print:             mov cl, byte [si]
                    cmp cl, 0
                    je .shutdown

                    mov byte [es:di], cl
                    add di, 1
                    mov byte [es:di], 0x04
                    add di, 1
                    add si, 1
                    jmp .print

.shutdown:          hlt
                    jmp .shutdown

;-----------------------------------------
; Disk Address Packet
;-----------------------------------------
DiskAddressPacket   db 0x10
                    db 0             ; 구조체 크기
                    dw 8             ; 읽고자 하는 섹터의 개수
                    dw RootDirectory ; offset
                    dw 0x0000        ; segment
StartSector         dq 0

;-----------------------------------------
; Etc Datas
;-----------------------------------------
ErrorMsg            db "Do not find out file..", 0
LoaderName          db "LOADER  ", "O  "

times 510-($-$$)    db 0x00
                    dw 0xAA55