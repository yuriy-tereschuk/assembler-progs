format ELF executable 3
entry _start

segment readable executable

_start:
  mov ecx, strlen    ; init loop counter
  mov esi, string    ; ponit source string
  stackpush:         ; copy source string in stack
  mov bl, byte [esi]
  cmp ebx, 61h
  jl skipuppercase
  sub ebx, 20h       ; to uppercase
  skipuppercase:
  push ebx
  inc esi
  loop stackpush     ; end copy in stack

  mov ecx, strlen    ; reinit loop counter
  mov edi, dst       ; point to destination memory

  stackpop:          ; copy from stack in dest memory
  pop ebx
  mov [edi], ebx
  inc edi
  loop stackpop      ; end copy from stack

segment readable writable
  string db "to be reversed"
  strlen = $ - string
  dst rb strlen

