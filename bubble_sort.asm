format ELF executable 3

include 'include/import32.inc'
include 'include/proc32.inc'

interpreter '/lib/ld-linux.so.2'
needed 'libc.so.6'
import printf,scanf,exit

segment executable

entry _start
_start:
  call bubble_sort

  mov ecx, array_size
  mov esi, unsorted
  mov ebx, unsorted_message
  call print_results

  mov ecx, array_size
  mov esi, sorted
  mov ebx, sorted_message
  call print_results

  cinvoke exit, 0h

bubble_sort:
  push ecx
  push edi
  push esi
  push eax

  xor ecx, ecx
  mov ecx, array_size
  mov esi, unsorted
  mov edi, sorted
  cld
  rep movsb

  xor eax, eax
  xor ecx, ecx
  mov ecx, array_size
  sort_array:
    push ecx
    mov ecx, array_size
    mov esi, sorted
    swap_entries:
      mov ah, [esi]
      mov al, [esi + 1]
      cmp ah, al
      jng skip_swap
        mov [esi], al
        mov [esi+1], ah
      skip_swap:
      inc esi
      cmp ecx, 2h
      jbe swap_iteration_compleated
      loop swap_entries
    swap_iteration_compleated:
    pop ecx
    loop sort_array

  pop eax
  pop esi
  pop edi
  pop ecx
  ret

print_results:
  push ecx
  push eax
  mov edi, array_string
  make_string:
    mov al, [esi]
    add al, 30h
    mov [edi], al
    cmp ecx, 1h
    je L0
    mov byte [edi+1], ','
    mov byte [edi+2], ' '
    inc esi
    add edi, 3
    L0:
    loop make_string
  cinvoke printf, ebx, array_string
  pop eax
  pop ecx
  ret


segment readable
  unsorted db 4, 3, 7, 2, 9, 5, 1, 6, 8, 0
  array_size = $ - unsorted
  unsorted_message db "Unordered array: %s", 0Ah, 0h
  sorted_message db "Ordered array: %s", 0Ah, 0h

segment writable
  sorted rb 10
  array_string rb array_size

