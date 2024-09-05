format ELF

section '.text' executable

public _start

define SYS_exit 1

_start:
  xor ecx, ecx
  mov esi, array
  mov dh, [esi]
  compare:
  add esi, ecx
  mov al, [esi]
  mov ah, [esi+1]
  mov bl, [esi+2]
  mov bh, [esi+3]
  mov dl, [esi+4]

  cmp dh, al
  jg L0
  mov dh, al              ; copy bigger value from register
  L0:
  cmp dh, ah
  jg L1
  mov dh, ah              ; copy bigger value from register
  L1:
  cmp dh, bl
  jg L2
  mov dh, bl              ; copy bigger value from register
  L2:
  cmp dh, bh
  jg L3
  mov dh, bh              ; copy bigger value from register
  L3:
  cmp dh, dl
  jg L4
  mov dh, dl              ; copy bigger value from register
  L4:
  add ecx, 5
  add esi, ecx
  cmp ecx, array_size
  jl compare

  mov [externum], dh      ; store externum value

  mov eax, SYS_exit
  mov ebx, dword [externum]
  int 80h


section '.data' writable
  array db 4, 7, 2, 3, 9, 5, 0, 1, 8, 6
  array_size = $ - array - 1

section '.bss' writable
  externum  rb 1
