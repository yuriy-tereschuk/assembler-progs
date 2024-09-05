format ELF

section '.text' executable

public _start

define SYS_exit 1

_start:
  mov ecx, array_size
  mov al, [array]
  mov [externum], al
  compare:
  mov al, [array+ecx]
  mov bl, [array+ecx-1]
  mov dl, [array+ecx-2]
  cmp al, bl
  jg L1
  mov al, bl              ; copy bigger value from register
  L1:
  cmp al, dl
  jg L2
  mov al, dl              ; copy bigger value from register
  L2:
  cmp [externum], al
  jg L3
  mov [externum], al
  L3:
  sub ecx, 3
  cmp ecx, 0h
  jnl compare

  mov eax, SYS_exit
  mov ebx, dword [externum]
  int 80h


section '.data' writable
  array db 4, 7, 2, 3, 9, 5, 0, 1, 8, 6
  array_size = $ - array - 1

section '.bss' writable
  externum  rb 1
