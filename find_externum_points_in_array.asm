format ELF

section '.text' executable

public _start

define SYS_exit 1

_start:
  mov eax, array
  mov [externum], al

  mov ecx, array_size
  compare:
  inc eax
  cmp [externum], al
  jg L2
  mov [externum], al        ; copy bigger value from register
  L2:
  dec ecx
  jnz compare

  mov eax, SYS_exit
  mov ebx, dword [externum]
  int 80h


section '.data' writable
  array db 4, 7, 2, 3, 9, 5, 0, 1, 8, 6
  array_size = $ - array - 1

section '.bss' writable
  externum  rb 1
