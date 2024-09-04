format ELF executable 3
entry _start

segment executable readable writable

_start:
  mov al, [a]
  mov bl, [b]
  mov cl, [c]
  mov [result], al
  cmp [result], bl
  jl @f
  mov [result], bl
  @@:
  cmp [result], cl
  jl @f
  mov [result], cl
  @@:
  ret

segment readable
  a db 1
  b db 2
  c db 3

segment readable writable
  result  rb 1
