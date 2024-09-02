format ELF executable 3
entry _start

segment readable executable

_start:
  mov al, [a]
  mov bl, [b]
  mov cl, [c]
  mov [d], al
  add [d], bl
  add [d], cl
  ret

segment readable
  
  a db 1
  b db 2
  c db 3

segment readable writable
  d rb 1
