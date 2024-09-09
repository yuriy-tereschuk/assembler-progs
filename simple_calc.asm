format ELF executable 3

include 'include/import32.inc'
include 'include/proc32.inc'

interpreter '/lib/ld-linux.so.2'
needed 'libc.so.6'
import printf,scanf,exit

segment executable

entry _start
_start:
  cinvoke printf, msg
  cinvoke printf, rst
  cinvoke scanf, rfa, arg_a, arg_b
  cinvoke printf, rfo
  cinvoke scanf, roa, operator

  call do_calc

  cinvoke exit

do_calc:
  push eax
  push ebx
  xor eax, eax
  xor ebx, ebx
  mov al, [operator]
  mov bh, [arg_a]
  mov bl, [arg_b]
  cmp al, '+'
  je do_add
  cmp al, '-'
  je do_sub
  cmp al, '*'
  je do_mul
  cmp al, '/'
  je do_div

  print_result:
  shr ebx, 8
  cinvoke printf, result_msg, eax, ebx

  pop ebx
  pop eax
  ret

do_add:
  add bh, bl
  jmp print_result

do_sub:
  sub bh, bl
  jmp print_result

do_mul:
  push eax
  mov al, bh
  mul bl
  mov bh, al
  pop eax
  jmp print_result

do_div:
  push eax
  mov al, bh
  div bl
  mov bh, al
  pop eax
  jmp print_result


segment readable

  msg db "Hello Simple Calc!", 0Ah, 0h
  rst db "Enter two arguments(in rage of 0-9) with space delimiter(a b):", 0h
  rfa db "%d %d", 0h
  rfo db "Enter arifmetic operation(*, /, +, -):", 0h
  roa db "%s", 0h
  result_msg db "Result of operation %c: %d", 0Ah, 0

segment writable

  arg_a rb 1
  arg_b rb 1
  operator rb 1
