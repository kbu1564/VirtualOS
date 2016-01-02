[org 0x9000]
[bits 16]

jmp _start

%include "print.asm"

_start:
  xor ax, ax
  mov es, ax
  mov ds, ax
  mov fs, ax
  mov gs, ax

  ; boot(0x7c00) -> booting!! -> find loader.sys -> jmp loader.asm:_start
  ; video memory(0xb8000) 여기서 일정 크기만큼의 공간의
  ; 메모리의 값을 화면에 지속적으로 출력!! - GPU
  ; ---------------
  ; | char | attr |
  ; ---------------

  ; c++ idiom, 아키텍쳐 패턴
  ; 이곳에서 미리 C언어 커널 파일을 로드한다.
  xor eax, eax
  xor ebx, ebx
  xor ecx, ecx
.loader:
  mov cl, byte [TotalFATs]
  mov eax, dword [BigSectorsPerFAT]
  mov bx, word [ReservedSectors]
  mul ecx
  ; dx:ax = ax * r16
  ; edx:eax = eax * r32
  add eax, ebx
  ; ax = ax + r16
  ;----------------------------------
  ; RootDir의 위치 : eax
  mov dword [StartSector], eax

  ; 디스크의 데이터를 읽어오기 위해 인터럽트를 호출한다.
  mov ah, 0x42
  mov dl, byte [BootDiskNumber]
  mov si, DiskAddressPacket
  int 0x13
  jc .shutdown
  ; RootDirEntry의 내용이 구해짐(폴더에 뭐있는지에 대한 목록이 구해짐)
  ; 0x8000 -> loader.sys 파일을 찾아야되 -> 데이터가 저장된 위치를 찾고
  ; -> 찾은 위치를 int 0x13 이용해서 0x8000번지 메모리에 적재 -> jmp 0x8000
  
  ; 읽은 RootDirEntry로 부터 loader.sys파일을 찾는다
  mov di, 0x8000
.find:
  ; 만약 첫번째 바이트가 0xE5 ?? => 삭제된 파일
  ; 삭제된게 아닐경우 파일이름 비교
  mov cl, byte [di]
  cmp cl, 0xE5
  je .next

  ; 이 부분이 실행된다는 것은 삭제된 파일이 아니라는 것이다.
  mov cx, 8
  mov bx, KernelName
  mov si, di
.compare:
  ; dx의 1바이트가 si의 1바이트와 같으면 루프 계속 다르면 다음 파일정보 찾기
  ; 주소 지정용 레지스터 : bx, si, di, bp, sp
  mov al, byte [bx]
  cmp al, byte [si]
  jne .next

  add bx, 1
  add si, 1
  loop .compare
  ; for (int i = ??; i > 0; i--) { ... }
  jmp .video

.next:
  add di, 0x20
  jmp .find

.video:
  xor eax, eax
  ; 이 시점에 위치한 상태인 경우 원하는 파일을 찾은 상태이다.
  mov ax, word [di + 20]
  shl eax, 16
  mov ax, word [di + 26]
  sub eax, 2
  ; 이때의 eax 값이 파일 정보가 저장된 클러스터 위치 값이다.
  mov ecx, 0
  mov cl, byte [SectorsPerCluster]
  mul ecx

  add eax, dword [StartSector]
  mov dword [StartSector], eax
  mov  word [DiskAddressPacket + 4], 0xA000

  ; 디스크의 데이터를 읽어오기 위해 인터럽트를 호출한다.
  mov ah, 0x42
  mov dl, byte [BootDiskNumber]
  mov si, DiskAddressPacket
  int 0x13
  jc .shutdown

  cli
  ;인터럽트 호출 중지
  
  lgdt [GDTR]
  ;Load GDT
  
  push HelloMsg
  push 160 * 1
  call kPrint

  push LoadGDTMsg
  push 160 * 2
  call kPrint

  ; 32bit 모드로 전환
  mov eax, cr0
  or eax, 0x00000001
  mov cr0, eax

  jmp $+2
  ; $는 현재 주소
  nop
  nop
  ; 혹시 남아 있을지도 모르는 명령어를 지우는 목적으로 nop을 쓴다.
  jmp dword CodeDescriptor:_protect_start

.shutdown:
  jmp .shutdown

; Bios Parameter Block
BPBArea                 equ 0x7C03
BytesPerSector          equ BPBArea + 8
SectorsPerCluster       equ BPBArea + 10
ReservedSectors         equ BPBArea + 11
TotalFATs               equ BPBArea + 13
BigSectorsPerFAT        equ BPBArea + 33
BootDiskNumber          equ BPBArea + 61

HelloMsg   db 'Hello, Loader!!', 0
LoadGDTMsg db 'Global Descriptor Table Load Success!!', 0
KernelName db 'MAIN    ', 'O  '
;-----------------------------------------
DiskAddressPacket:
             db 0x10, 0 ; 구조체 크기
             dw 8       ; 읽고자 하는 섹터의 개수
             dw 0x8000
             dw 0x0000
StartSector: dq 0
;-----------------------------------------

GDTR:
  dw GDTEND - GDT - 1
  dd GDT
GDT:
NullDescriptor equ 0x00
  dw 0x0000
  ; limit
  dw 0x0000
  ; baseaddr 
  db 0x00
  ; baseaddr
  db 0x00
  ; P DPL S TYPE
  db 0x00
  ; G D AVL limit
  db 0x00
  ; baseaddr

CodeDescriptor equ 0x08
  dw 0xFFFF
  dw 0x0000
  db 0x00
  db 0x9A
  db 0xCF
  db 0x00
  
DataDescriptor equ 0x10
  dw 0xFFFF
  dw 0x0000
  db 0x00
  db 0x92
  db 0xCF
  db 0x00

VideoDescriptor equ 0x18
  dw 0xFFFF
  dw 0x8000
  db 0x0B
  db 0x92
  db 0x4F
  db 0x00
GDTEND:

[bits 32]

%include "print32.asm"

_protect_start:
  mov ax, DataDescriptor
  mov es, ax
  mov ds, ax
  mov fs, ax
  mov gs, ax

  push ProtectModeMsg
  push 160 * 3
  ; 1줄에 최대 80문자
  ; 비디오 메모리에서 2byte이므로 80*2 = 160
  ; 3은 몇번째 줄인지
  call kPrint32

  jmp 0xA000

.shutdown:
  jmp .shutdown

ProtectModeMsg db 'Hello, ProtectMode!!', 0

