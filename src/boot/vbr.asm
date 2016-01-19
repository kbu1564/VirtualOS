[bits 16]
[org 0x7C00]

jmp main
nop

GPTAddress          equ 0x0600
RDAddress           equ 0xA000
FATArea             equ 0x1000
LoadAddress         equ 0xA000

; File Allocation Table
FileName            equ 0x00
FilenameExtension   equ 0x08
FileFlag            equ 0x0B
Unused              equ 0x0C
HightStartCluster   equ 0x14
Time                equ 0x16
Date                equ 0x18
LowStartCluster     equ 0x1A
FileSize            equ 0x1C

;-----------------------------------------
; File Allocation Table
;-----------------------------------------
OemID               db "MSDOS5.0"
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
main:               xor eax, eax
                    mov es, ax
                    mov ds, ax
                    mov ss, ax
                    mov fs, ax
                    mov gs, ax
                    mov sp, 0xFFF8

                    ; 이곳에서 미리 C언어 커널 파일을 로드한다.
.loader:            movzx ecx, byte [TotalFATs]
                    mov eax, dword [BigSectorsPerFAT]
                    movzx ebx, word [ReservedSectors]
                    mul ecx
                    ; dx:ax = ax * r16
                    ; edx:eax = eax * r32
                    add eax, ebx

                    ; Supported MBR and GPT (GPT or not thing)
                    cmp word [GPTAddress + 510], 0xAA55
                    jne .mbrboot1
                    add eax, dword [HiddenSectors]

                    ; ax = ax + r16
                    ;----------------------------------
                    ; RootDir의 위치 : eax

                    ; file search!!
                    ;------------------------------------------------------------------------
.mbrboot1:          mov dword [DiskAddressPacket + 8], eax
                    mov dword [RDStartSector], eax

                    ; 디스크의 데이터를 읽어오기 위해 인터럽트를 호출한다.
                    mov ah, 0x42
                    mov dl, byte [BootDiskNumber]
                    mov si, DiskAddressPacket
                    int 0x13
                    jc .failed

                    ; 읽은 RootDirEntry로 부터 파일을 찾는다
                    mov di, RDAddress
.find:              mov cl, byte [di]
                    cmp cl, 0xE5
                    je .next
                    ; 만약 첫번째 바이트가 0xE5 ?? => 삭제된 파일
                    ; 삭제된게 아닐경우 파일이름 비교
                    test cl, cl
                    jz .failed

                    ; 이 부분이 실행된다는 것은 삭제된 파일이 아니라는 것이다.
                    mov cx, 8
                    mov bx, KernelName
                    mov si, di
.compare:           mov al, byte [bx]
                    cmp al, byte [si]
                    jne .next
                    ; dx의 1바이트가 si의 1바이트와 같으면 루프 계속 다르면 다음 파일정보 찾기
                    ; 주소 지정용 레지스터 : bx, si, di, bp, sp

                    inc bx
                    inc si
                    loop .compare
                    ; for (int i = ??; i > 0; i--) { ... }
                    jmp .readcluster

.next:              add di, 0x20
                    jmp .find
                    ;------------------------------------------------------------------------

.readcluster:       xor eax, eax
                    ; 이 시점에 위치한 상태인 경우 원하는 파일을 찾은 상태이다.
                    ;------------------------------------------------------------------------
                    ; Cluster Linked List (FAT 찾기)
                    ; ClusterLinkedList = ReservedSectors + BigSectorsPerFAT + HiddenSectors
                    mov ax, word [ReservedSectors]
                    add eax, dword [BigSectorsPerFAT]

                    ; Supported MBR and GPT (GPT or not thing)
                    cmp word [GPTAddress + 510], 0xAA55
                    jne .mbrboot2
                    add eax, dword [HiddenSectors]

.mbrboot2:          mov dword [DiskAddressPacket + 8], eax

                    xor cx, cx
                    mov cl, byte [SectorsPerCluster]
                    mov word [DiskAddressPacket + 2], cx
                    mov word [DiskAddressPacket + 4], FATArea

                    ; 디스크의 데이터를 읽어오기 위해 인터럽트를 호출한다.
                    mov ah, 0x42
                    mov dl, byte [BootDiskNumber]
                    mov si, DiskAddressPacket
                    int 0x13
                    jc .failed
                    ;------------------------------------------------------------------------

                    ; 1cluster가 몇 bytes 인지 계산
                    movzx ecx, byte [SectorsPerCluster]
                    movzx eax, word [BytesPerSector]
                    mul ecx
                    mov dword [BytesPerCluster], eax

                    ; file start cluster
                    mov ax, word [di + HightStartCluster]
                    shl eax, 16
                    mov ax, word [di + LowStartCluster]

                    ; FAT index list 를 이용하여 파일 전부 읽기
                    ; get next cluster pointer
                    ;------------------------------------------------------------------------
                    ; 커널파일은 LoadAddress 번지에 로드
                    mov ebx, LoadAddress
.run:               mov ecx, eax
                    and ecx, 0x0FFFFFF8
                    cmp ecx, 0x0FFFFFF8
                    je .eof

                    ; 2016-01-19 : 올바르게 클러스터에 대한 파일 정보가 로드되지 않고 있음
                    ;   원인) Cluster Linked Array와 Root Directory Entry Table의 로드 주소가 같아서 발생
                    ; 2016-01-19 : 이후 realmode를 위한 putc(char c); 함수 필요
                    ; get next cluster index
                    ; edi = FATArea + edi * 4
                    mov edi, eax
                    shl edi, 2
                    add edi, FATArea

                    sub eax, dword [RootDirectoryStart]
                    ; 이때의 eax 값이 파일 정보가 저장된 RootDirectory 기준으로 부터의 클러스터 위치 값이다.

                    ; cluster 단위 -> sector 단위로 변환
                    movzx ecx, byte [SectorsPerCluster]
                    mul ecx ; eax = eax * ecx
                    add eax, dword [RDStartSector]

                    mov dword [DiskAddressPacket + 8], eax ; read sector

                    mov eax, ebx
                    and eax, 0xFFFF0000
                    shr eax, 1
                    ; 1 cluster 읽기
                    mov word [DiskAddressPacket + 4], bx   ; offset
                    mov word [DiskAddressPacket + 6], ax   ; segment

                    ; 디스크의 데이터를 읽어오기 위해 인터럽트를 호출한다.
                    mov ah, 0x42
                    mov dl, byte [BootDiskNumber]
                    mov si, DiskAddressPacket
                    int 0x13
                    jc .failed

                    ; next loop
                    add ebx, dword [BytesPerCluster]
                    ; next save memory address = ebx + (SectorPerCluster * BytePerSector)

                    ; next cluster entry point
                    mov eax, dword [edi]
                    jmp .run
                    ;------------------------------------------------------------------------

                    ; eof(end of file)
.eof:               cli
                    ;인터럽트 호출 중지

                    jmp $+2
                    ; $는 현재 주소
                    nop
                    nop
                    ; 혹시 남아 있을지도 모르는 명령어를 지우는 목적으로 nop을 쓴다.
                    jmp dword 0:LoadAddress

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
                    inc di
                    inc si
                    jmp .print

.shutdown:          hlt
                    jmp .shutdown

BytesPerCluster     dd 0 ; 속도 향상을 위한 1cluster = ?bytes 기록
RDStartSector       dd 0 ; 속도 향상을 위한 RootDirectory 기록
KernelName          db 'KERNEL  ', 'O  '
;-----------------------------------------
; Disk Address Packet
;-----------------------------------------
DiskAddressPacket   db 0x10, 0       ; 구조체 크기
                    dw 8             ; 읽고자 하는 섹터의 개수
                    dw RDAddress     ; offset
                    dw 0x0000        ; segment
                    dq 0

;-----------------------------------------
; Etc Datas
;-----------------------------------------
ErrorMsg            db "read file error!!", 0

times 510-($-$$)    db 0x00
                    dw 0xAA55
