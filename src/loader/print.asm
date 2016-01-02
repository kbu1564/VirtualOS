kPrint:
  push bp
  mov bp, sp
  ; 0xb8000 -> (char)'H' -> 1bite 쓰여짐!!
  ; *((char*)0xb8000) = 'H';
  ; 0xb8001 -> (char)0x04 -> 1bite 쓰여짐!!
  ; *((char*)0xb8001) = 0x04;
 
  pusha

  ; 16bit -> 0xFFFF
  mov ax, 0xb800
  mov es, ax
  ; 일단 GDT를 이용하지 말자!
  ; GDT 활성화 이후 Video Descripter

  mov si, word[bp + 4]
  mov di, word[bp + 6]
.print:
  ; cx = ch + cl
  mov cl, byte [di]
  cmp cl, 0
  je .ret

  mov byte [es:si], cl
  add si, 1
  mov byte [es:si], 0x04
  add si, 1
  add di, 1
  jmp .print

.ret:
  mov ax, 0x10
  mov es, ax
  popa
  mov sp, bp
  pop bp
  ret 4

