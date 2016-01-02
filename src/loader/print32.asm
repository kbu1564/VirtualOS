section .data
section .text

global kPrint32

kPrint32:
  push ebp
  mov ebp, esp
  ; 0xb8000 -> (char)'H' -> 1bite 쓰여짐!!
  ; *((char*)0xb8000) = 'H';
  ; 0xb8001 -> (char)0x04 -> 1bite 쓰여짐!!
  ; *((char*)0xb8001) = 0x04;
  pusha

  ; 16bit -> 0xFFFF
  mov eax, 0x18
  mov es, eax
  ; 일단 GDT를 이용하지 말자!
  ; GDT 활성화 이후 Video Descripter

  mov esi, dword[ebp + 8]
  mov edi, dword[ebp + 12]
.print:
  ; cx = ch + cl
  mov cl, byte [di]
  cmp cl, 0
  je .ret

  mov byte [es:esi], cl
  add si, 1
  mov byte [es:esi], 0x04
  add esi, 1
  add edi, 1
  jmp .print

.ret:
  mov eax, 0x10
  mov es, eax

  popa
  mov esp, ebp
  pop ebp
  ret 8

