format ELF executable 3
entry _start

segment executable readable writable

_start:
  mov al, [a]
  mov bl, [b]
  mov cl, [c]
  mov [maxvalue], al
  cmp [maxvalue], bl
  jg @f
  mov [maxvalue], bl
  @@:
  cmp [maxvalue], cl
  jg @f
  mov [maxvalue], cl
  @@:
  ret

segment readable
  a db 1
  b db 2
  c db 3

segment readable writable
  maxvalue  rb 1
