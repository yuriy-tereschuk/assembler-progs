format ELF executable 3

include 'include/import32.inc'
include 'include/proc32.inc'

interpreter '/lib/ld-linux.so.2'
needed 'libc.so.6'
import printf,exit

segment readable executable

entry _start
_start:
    call read_word
    
    call check_input

    call loop_factorial
    mov ebx, loperation
    mov ecx, [lfactorial]
    call print_result

    call recursion_factorial
    mov ebx, roperation
    mov ecx, [rfactorial]
    call print_result

    cinvoke exit

read_word:
  mov eax, 7
  ret

print_result:
  cinvoke printf, message, ebx, eax, ecx
  ret

check_input:    ; skip factorial for 0, 1, 2 and negative values
  cmp eax, 2h
  jae L1
  cinvoke exit
  L1:
  ret

loop_factorial:
  push ecx
  push eax      ; store function argument
  mov ecx, eax
  dec ecx
  factorial:
    cmp ecx, 1h
    je L0
    mul ecx
    loop factorial

  L0:
    mov [lfactorial], eax

  pop eax       ; restore arg value
  pop ecx
  ret

recursion_factorial:
  push eax      ; store function argumen
  mov eax, 0

  pop eax       ; restore args value
  ret

segment readable writable

  lfactorial dd 1
  rfactorial dd 1

  message db "%s factorial of %d: %d", 0Ah, 0h
  loperation db "Loop", 0h
  roperation db "Recursion", 0h

